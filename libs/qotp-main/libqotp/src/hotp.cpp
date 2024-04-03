#include <libqotp/qotp.h>

#include <cmath>
#include <QMessageAuthenticationCode>

// Refer to the detailed documentation in qotp.h for complete information about this function.
QString libqotp::hotp(
    QByteArrayView secret,
    uint64_t counter,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum,
    QCryptographicHash::Algorithm algorithm)
{
   // Input validation
   if (secret.isEmpty())
   {
      // An empty secret key is invalid as it compromises the security of the OTP.
      // The shared secret must be kept confidential between the token creator and the token verifier.
      return QString();
   }

   if (digits < digitMinimum || digits > digitMaximum)
   {
      // The RFC 4226 recommends the output OTP to be at least 6 digits long.
      // Digits more than 8 are often not supported by OTP systems and may be harder for users.
      return QString();
   }

   // Counter value to byte array conversion
   QByteArray counterBytes;
   for (int i = 7; i >= 0; --i)
   {
      counterBytes.append((counter >> (i * 8)) & 0xFF);
   }

   QByteArray hash;

   switch (algorithm)
   {
   case QCryptographicHash::Sha1:
   {
      // Calculate HMAC-SHA1 hash
      hash = QMessageAuthenticationCode::hash(counterBytes, secret.toByteArray(), QCryptographicHash::Sha1);

      // Check for valid hash
      if (hash.isEmpty() || hash.length() < 20)
      {
         // An invalid hash indicates an error in the HMAC computation.
         // The length of SHA-1 hash should always be 20 bytes.
         return QString();
      }
      break;
   }
   case QCryptographicHash::Sha256:
   {
      // Calculate HMAC-SHA256 hash
      hash = QMessageAuthenticationCode::hash(counterBytes, secret.toByteArray(), QCryptographicHash::Sha256);

      // Check for valid hash
      if (hash.isEmpty() || hash.length() < 32)
      {
         // An invalid hash indicates an error in the HMAC computation.
         // The length of SHA-256 hash should always be 32 bytes.
         return QString();
      }
      break;
   }
   case QCryptographicHash::Sha512:
   {
      // Calculate HMAC-SHA512 hash
      hash = QMessageAuthenticationCode::hash(counterBytes, secret.toByteArray(), QCryptographicHash::Sha512);

      // Check for valid hash
      if (hash.isEmpty() || hash.length() < 64)
      {
         // An invalid hash indicates an error in the HMAC computation.
         // The length of SHA-512 hash should always be 64 bytes.
         return QString();
      }
      break;
   }
   default:
      break;
   }

   // Check for valid hash
   if (hash.isEmpty())
   {
      return QString();
   }

   // Dynamic Truncation
   int offset = hash.at(hash.length() - 1) & 0xf;
   if (offset > hash.length() - 4)
   {
      // Ensuring offset is within the bounds of the hash array to prevent out-of-bounds access.
      // The offset calculation is based on the last byte of the hash and must allow for subsequent bytes.
      return QString();
   }

   quint32 truncatedHash = (static_cast<quint32>(hash.at(offset) & 0x7f) << 24) |
                           (static_cast<quint32>(hash.at(offset + 1) & 0xff) << 16) |
                           (static_cast<quint32>(hash.at(offset + 2) & 0xff) << 8) |
                           (static_cast<quint32>(hash.at(offset + 3) & 0xff));

   // Generate HOTP value
   quint32 hotp = truncatedHash % static_cast<quint32>(std::pow(10, digits));

   // Return HOTP as zero-padded string
   return QString::number(hotp).rightJustified(digits, '0');
}

// Refer to the detailed documentation in qotp.h for complete information about this function.
QString libqotp::hotp_base32(
    const QString &base32,
    uint64_t counter,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum,
    QCryptographicHash::Algorithm algorithm)
{
   // Decode the Base32 secret
   QByteArray secret = libqotp::base32_decode(base32);

   // Call the original hotp function with the decoded secret
   return libqotp::hotp(QByteArrayView(secret), counter, digits, digitMinimum, digitMaximum, algorithm);
}

// Refer to the detailed documentation in qotp.h for complete information about this function.
QString libqotp::hotp_base64(
    const QByteArray &base64,
    uint64_t counter,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum,
    QCryptographicHash::Algorithm algorithm,
    QByteArray::Base64Options options)
{
   // Decode the Base64 secret
   QByteArray secret = QByteArray::fromBase64(base64, options);

   // Call the original hotp function with the decoded secret
   return libqotp::hotp(QByteArrayView(secret), counter, digits, digitMinimum, digitMaximum, algorithm);
}
