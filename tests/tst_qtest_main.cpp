#include <QTest>
#include <QObject>
#include <QElapsedTimer>
#include "HWConfigDialog.h"
#include "CANConfigWidget.h"

class SimpleQtTest : public QObject
{
    Q_OBJECT

private slots:
    void toUpper();
    void hwConfigDialog_construct_show_close_noCrash();
    void canDetectHw_returnsQuickly();
};

void SimpleQtTest::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

void SimpleQtTest::hwConfigDialog_construct_show_close_noCrash()
{
    HWConfigDialog dialog;
    dialog.show();

    // Let queued UI work run (e.g., CAN channel detection invokeMethod).
    QTest::qWait(250);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 250);

    dialog.close();
    QVERIFY(!dialog.isVisible());
}

void SimpleQtTest::canDetectHw_returnsQuickly()
{
    CANConfigWidget widget;

    QElapsedTimer timer;
    timer.start();
    widget.refreshVectorChannels();

    // Detect HW must never block the UI thread.
    QVERIFY2(timer.elapsed() < 500, "Detect HW blocked the UI thread");
}

QTEST_MAIN(SimpleQtTest)
#include "tst_qtest_main.moc"
