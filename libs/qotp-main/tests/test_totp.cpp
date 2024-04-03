#include <QtTest>

#include <libqotp/qotp.h>

class test_totp : public QObject
{
   Q_OBJECT

private slots:
   void test_match_rfc_sha1()
   {
      const auto key = QByteArrayView("12345678901234567890");
      QCOMPARE(libqotp::totp(key, 1111111109), QLatin1String("07081804"));
      QCOMPARE(libqotp::totp(key, 1111111111), QLatin1String("14050471"));
      QCOMPARE(libqotp::totp(key, 1234567890), QLatin1String("89005924"));
      QCOMPARE(libqotp::totp(key, 2000000000), QLatin1String("69279037"));
      QCOMPARE(libqotp::totp(key, 20000000000), QLatin1String("65353130"));
   }

   void test_match_rfc_sha1_base32()
   {
      const auto key = QLatin1String("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ");
      QCOMPARE(libqotp::totp_base32(key, 1111111109), QLatin1String("07081804"));
      QCOMPARE(libqotp::totp_base32(key, 1111111111), QLatin1String("14050471"));
      QCOMPARE(libqotp::totp_base32(key, 1234567890), QLatin1String("89005924"));
      QCOMPARE(libqotp::totp_base32(key, 2000000000), QLatin1String("69279037"));
      QCOMPARE(libqotp::totp_base32(key, 20000000000), QLatin1String("65353130"));
   }

   void test_match_rfc_sha1_base64()
   {
      const auto key = QLatin1String("MTIzNDU2Nzg5MDEyMzQ1Njc4OTA=").toString().toUtf8();;
      QCOMPARE(libqotp::totp_base64(key, 1111111109), QLatin1String("07081804"));
      QCOMPARE(libqotp::totp_base64(key, 1111111111), QLatin1String("14050471"));
      QCOMPARE(libqotp::totp_base64(key, 1234567890), QLatin1String("89005924"));
      QCOMPARE(libqotp::totp_base64(key, 2000000000), QLatin1String("69279037"));
      QCOMPARE(libqotp::totp_base64(key, 20000000000), QLatin1String("65353130"));
   }

   void test_match_rfc_sha256()
   {
      const auto key = QByteArrayView("12345678901234567890123456789012");
      QCOMPARE(libqotp::totp_sha256(key, 1111111109), QLatin1String("68084774"));
      QCOMPARE(libqotp::totp_sha256(key, 1111111111), QLatin1String("67062674"));
      QCOMPARE(libqotp::totp_sha256(key, 1234567890), QLatin1String("91819424"));
      QCOMPARE(libqotp::totp_sha256(key, 2000000000), QLatin1String("90698825"));
      QCOMPARE(libqotp::totp_sha256(key, 20000000000), QLatin1String("77737706"));
   }

   void test_match_rfc_sha256_base32()
   {
      const auto key = QLatin1String("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZA====");
      QCOMPARE(libqotp::totp_base32_sha256(key, 1111111109), QLatin1String("68084774"));
      QCOMPARE(libqotp::totp_base32_sha256(key, 1111111111), QLatin1String("67062674"));
      QCOMPARE(libqotp::totp_base32_sha256(key, 1234567890), QLatin1String("91819424"));
      QCOMPARE(libqotp::totp_base32_sha256(key, 2000000000), QLatin1String("90698825"));
      QCOMPARE(libqotp::totp_base32_sha256(key, 20000000000), QLatin1String("77737706"));
   }

   void test_match_rfc_sha256_base64()
   {
      const auto key = QLatin1String("MTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTI=").toString().toUtf8();;
      QCOMPARE(libqotp::totp_base64_sha256(key, 1111111109), QLatin1String("68084774"));
      QCOMPARE(libqotp::totp_base64_sha256(key, 1111111111), QLatin1String("67062674"));
      QCOMPARE(libqotp::totp_base64_sha256(key, 1234567890), QLatin1String("91819424"));
      QCOMPARE(libqotp::totp_base64_sha256(key, 2000000000), QLatin1String("90698825"));
      QCOMPARE(libqotp::totp_base64_sha256(key, 20000000000), QLatin1String("77737706"));
   }

   void test_match_rfc_sha512()
   {
      const auto key = QByteArrayView("1234567890123456789012345678901234567890123456789012345678901234");
      QCOMPARE(libqotp::totp_sha512(key, 1111111109), QLatin1String("25091201"));
      QCOMPARE(libqotp::totp_sha512(key, 1111111111), QLatin1String("99943326"));
      QCOMPARE(libqotp::totp_sha512(key, 1234567890), QLatin1String("93441116"));
      QCOMPARE(libqotp::totp_sha512(key, 2000000000), QLatin1String("38618901"));
      QCOMPARE(libqotp::totp_sha512(key, 20000000000), QLatin1String("47863826"));
   }

   void test_match_rfc_sha512_base32()
   {
      const auto key = QLatin1String("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQGEZDGNA=");
      QCOMPARE(libqotp::totp_base32_sha512(key, 1111111109), QLatin1String("25091201"));
      QCOMPARE(libqotp::totp_base32_sha512(key, 1111111111), QLatin1String("99943326"));
      QCOMPARE(libqotp::totp_base32_sha512(key, 1234567890), QLatin1String("93441116"));
      QCOMPARE(libqotp::totp_base32_sha512(key, 2000000000), QLatin1String("38618901"));
      QCOMPARE(libqotp::totp_base32_sha512(key, 20000000000), QLatin1String("47863826"));
   }

   void test_match_rfc_sha512_base64()
   {
      const auto key = QLatin1String("MTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNA==").toString().toUtf8();
      QCOMPARE(libqotp::totp_base64_sha512(key, 1111111109), QLatin1String("25091201"));
      QCOMPARE(libqotp::totp_base64_sha512(key, 1111111111), QLatin1String("99943326"));
      QCOMPARE(libqotp::totp_base64_sha512(key, 1234567890), QLatin1String("93441116"));
      QCOMPARE(libqotp::totp_base64_sha512(key, 2000000000), QLatin1String("38618901"));
      QCOMPARE(libqotp::totp_base64_sha512(key, 20000000000), QLatin1String("47863826"));
   }

   void test_totp_expire_time()
   {
      unsigned int timeStep = 30;

      // Test case with a specific timestamp
      quint64 currentUnixTime = 1111111109;     // Specific Unix timestamp
      quint64 expectedExpireTime = 1111111110;  // Expected expiration time
      QCOMPARE(libqotp::totp_expire_time(currentUnixTime, 0, timeStep), expectedExpireTime);

      // Additional test cases with different timestamps
      QCOMPARE(libqotp::totp_expire_time(1111111111, 0, timeStep), 1111111140);
      QCOMPARE(libqotp::totp_expire_time(1234567890, 0, timeStep), 1234567890 + timeStep);
      QCOMPARE(libqotp::totp_expire_time(2000000000, 0, timeStep), 2000000010);
      QCOMPARE(libqotp::totp_expire_time(20000000000, 0, timeStep), 20000000010);

      // Test case with a non-zero epoch
      quint64 epoch = 100000; // Non-zero epoch
      QCOMPARE(libqotp::totp_expire_time(1111111109, epoch, timeStep), 1111111120);

      // Test case with a different time step
      unsigned int differentTimeStep = 60; // Different time step
      QCOMPARE(libqotp::totp_expire_time(1111111109, 0, differentTimeStep), 1111111140);
   }
};

QTEST_MAIN(test_totp)

#include "test_totp.moc"
