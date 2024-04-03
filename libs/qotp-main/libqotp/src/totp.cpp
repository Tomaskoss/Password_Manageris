#include <libqotp/qotp.h>

// Refer to the detailed documentation in qotp.h for complete information about this function.
QString libqotp::totp(
    QByteArrayView secret,
    quint64 currentUnixTime,
    unsigned int timeStep,
    quint64 epoch,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum,
    QCryptographicHash::Algorithm algorithm)
{
    // Ensure timeStep is not zero to avoid division by zero
    if (timeStep == 0) {
        return QString();
    }

    // Calculate the counter value based on the current time
    quint64 counter = (currentUnixTime - epoch) / timeStep;

    // Call the HOTP function using the calculated counter
    return libqotp::hotp(secret, counter, digits, digitMinimum, digitMaximum, algorithm);
}

// Refer to the detailed documentation in qotp.h for complete information about this function.
quint64 libqotp::totp_expire_time(
   quint64 currentUnixTime,
   quint64 epoch,
   unsigned int timeStep)
{
   // Calculate the time elapsed since the epoch
   quint64 timeSinceEpoch = currentUnixTime - epoch;

   // Calculate the start of the current time window
   quint64 windowStart = timeSinceEpoch - (timeSinceEpoch % timeStep);

   // Calculate and return the end of the current time window
   return epoch + windowStart + timeStep;
}

// Convenience
QString libqotp::totp_sha256(
    QByteArrayView secret,
    quint64 currentUnixTime,
    unsigned int timeStep,
    quint64 epoch,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum)
{
   return libqotp::totp(secret, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha256);
}

// Convenience
QString libqotp::totp_sha512(
    QByteArrayView secret,
    quint64 currentUnixTime,
    unsigned int timeStep,
    quint64 epoch,
    unsigned int digits,
    unsigned int digitMinimum,
    unsigned int digitMaximum)
{
   return libqotp::totp(secret, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha512);
}

// Convenience
QString libqotp::totp_base32(
   const QString& base32,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum,
   QCryptographicHash::Algorithm algorithm)
{
   // Decode the Base32 secret
   QByteArray secret = libqotp::base32_decode(base32);

   // Call the original hotp function with the decoded secret
   return libqotp::totp(QByteArrayView(secret), currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, algorithm);
}

// Convenience
QString libqotp::totp_base32_sha256(
   const QString& base32,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum)
{
   return libqotp::totp_base32(base32, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha256);
}

// Convenience
QString libqotp::totp_base32_sha512(
   const QString& base32,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum)
{
   return libqotp::totp_base32(base32, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha512);
}

// Convenience
QString libqotp::totp_base64(
   const QByteArray& base64,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum,
   QCryptographicHash::Algorithm algorithm,
   QByteArray::Base64Options options)
{
   // Decode the Base64 secret
   QByteArray secret = QByteArray::fromBase64(base64, options);

   // Call the original totp function with the decoded secret
   return libqotp::totp(QByteArrayView(secret), currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, algorithm);
}

// Convenience
QString libqotp::totp_base64_sha256(
   const QByteArray& base64,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum,
   QByteArray::Base64Options options)
{
   return libqotp::totp_base64(base64, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha256, options);
}

// Convenience
QString libqotp::totp_base64_sha512(
   const QByteArray& base64,
   quint64 currentUnixTime,
   unsigned int timeStep,
   quint64 epoch,
   unsigned int digits,
   unsigned int digitMinimum,
   unsigned int digitMaximum,
   QByteArray::Base64Options options)
{
   return libqotp::totp_base64(base64, currentUnixTime, timeStep, epoch, digits, digitMinimum, digitMaximum, QCryptographicHash::Sha512, options);
}
