#include <QtTest>

#include <libqotp/qotp.h>

class test_hotp : public QObject
{
   Q_OBJECT

private slots:
   void test_match_rfc()
   {
      const auto key = QByteArrayView("12345678901234567890");
      QCOMPARE(libqotp::hotp(key, 0), QLatin1String("755224"));
      QCOMPARE(libqotp::hotp(key, 1), QLatin1String("287082"));
      QCOMPARE(libqotp::hotp(key, 2), QLatin1String("359152"));
      QCOMPARE(libqotp::hotp(key, 3), QLatin1String("969429"));
      QCOMPARE(libqotp::hotp(key, 4), QLatin1String("338314"));
      QCOMPARE(libqotp::hotp(key, 5), QLatin1String("254676"));
      QCOMPARE(libqotp::hotp(key, 6), QLatin1String("287922"));
      QCOMPARE(libqotp::hotp(key, 7), QLatin1String("162583"));
      QCOMPARE(libqotp::hotp(key, 8), QLatin1String("399871"));
      QCOMPARE(libqotp::hotp(key, 9), QLatin1String("520489"));
   }

   void test_match_rfc_base32()
   {
      const auto key = QLatin1String("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ");
      QCOMPARE(libqotp::hotp_base32(key, 0), QLatin1String("755224"));
      QCOMPARE(libqotp::hotp_base32(key, 1), QLatin1String("287082"));
      QCOMPARE(libqotp::hotp_base32(key, 2), QLatin1String("359152"));
      QCOMPARE(libqotp::hotp_base32(key, 3), QLatin1String("969429"));
      QCOMPARE(libqotp::hotp_base32(key, 4), QLatin1String("338314"));
      QCOMPARE(libqotp::hotp_base32(key, 5), QLatin1String("254676"));
      QCOMPARE(libqotp::hotp_base32(key, 6), QLatin1String("287922"));
      QCOMPARE(libqotp::hotp_base32(key, 7), QLatin1String("162583"));
      QCOMPARE(libqotp::hotp_base32(key, 8), QLatin1String("399871"));
      QCOMPARE(libqotp::hotp_base32(key, 9), QLatin1String("520489"));
   }

   void test_match_rfc_base32_padding()
   {
      const auto key_base32 = QLatin1String("IFBEGRCFIY======");
      const auto key_decoded = QByteArrayView("ABCDEF");
      QCOMPARE(libqotp::hotp_base32(key_base32, 0), libqotp::hotp(key_decoded, 0));
   }

   void test_match_rfc_base64()
   {
      const auto key = QLatin1String("MTIzNDU2Nzg5MDEyMzQ1Njc4OTA=").toString().toUtf8();
      QCOMPARE(libqotp::hotp_base64(key, 0), QLatin1String("755224"));
      QCOMPARE(libqotp::hotp_base64(key, 1), QLatin1String("287082"));
      QCOMPARE(libqotp::hotp_base64(key, 2), QLatin1String("359152"));
      QCOMPARE(libqotp::hotp_base64(key, 3), QLatin1String("969429"));
      QCOMPARE(libqotp::hotp_base64(key, 4), QLatin1String("338314"));
      QCOMPARE(libqotp::hotp_base64(key, 5), QLatin1String("254676"));
      QCOMPARE(libqotp::hotp_base64(key, 6), QLatin1String("287922"));
      QCOMPARE(libqotp::hotp_base64(key, 7), QLatin1String("162583"));
      QCOMPARE(libqotp::hotp_base64(key, 8), QLatin1String("399871"));
      QCOMPARE(libqotp::hotp_base64(key, 9), QLatin1String("520489"));
   }

   void test_invalid_inputs()
   {
      const auto key = QByteArrayView("12345678901234567890");

      // Test with an empty secret
      QCOMPARE(libqotp::hotp(QByteArrayView(""), 0), QString());

      // Test with digits less than the minimum allowed
      QCOMPARE(libqotp::hotp(key, 0, 4), QString());

      // Test with digits more than the maximum allowed
      QCOMPARE(libqotp::hotp(key, 0, 10), QString());

      // Test with digitMinimum greater than digitMaximum
      QCOMPARE(libqotp::hotp(key, 0, 6, 8, 7), QString());

      // Test with digitMinimum less than global minimum
      QCOMPARE(libqotp::hotp(key, 0, 6, 7), QString());

      // Test with digitMaximum more than global maximum
      QCOMPARE(libqotp::hotp(key, 0, 6, 9, 6), QString());

      // Test with requested digits outside the range of digitMinimum and digitMaximum
      QCOMPARE(libqotp::hotp(key, 0, 9, 7, 8), QString());
   }
};

QTEST_MAIN(test_hotp)

#include "test_hotp.moc"
