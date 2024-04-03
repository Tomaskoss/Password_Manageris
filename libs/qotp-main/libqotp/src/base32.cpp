#include <libqotp/qotp.h>

#include <QMap>

// Refer to the detailed documentation in qotp.h for complete information about this function.
QByteArray libqotp::base32_decode(const QString &base32String)
{
   static const QMap<char, quint8> base32Alphabet = {
      {'A', 0}   , {'B', 1}  , {'C', 2}  , {'D', 3}   , {'E', 4}  , {'F', 5}  , {'G', 6}  , {'H', 7},
      {'I', 8}   , {'J', 9}  , {'K', 10} , {'L', 11}  , {'M', 12} , {'N', 13} , {'O', 14} ,
      {'P', 15}  , {'Q', 16} , {'R', 17} , {'S', 18}  , {'T', 19} , {'U', 20} , {'V', 21} ,
      {'W', 22}  , {'X', 23} , {'Y', 24} , {'Z', 25}  , {'2', 26} , {'3', 27} , {'4', 28} ,
      {'5', 29}  , {'6', 30} , {'7', 31}
   };

   QByteArray decoded;
   int bitBuffer = 0;
   int currentBits = 0;
   int paddingCount = 0;

   for (QChar c : base32String.toUpper())
   {
      if (c == '=')
      {
         // Padding character
         paddingCount++;
         continue;
      }

      if (!base32Alphabet.contains(c.toLatin1()))
      {
         // Invalid character encountered
         return QByteArray();
      }

      if (paddingCount > 0)
      {
         // Any character after a padding character is invalid
         return QByteArray();
      }

      bitBuffer = (bitBuffer << 5) | base32Alphabet[c.toLatin1()];
      currentBits += 5;

      if (currentBits >= 8)
      {
         decoded.append(static_cast<char>((bitBuffer >> (currentBits - 8)) & 0xFF));
         currentBits -= 8;
      }
   }

   return decoded;
}
