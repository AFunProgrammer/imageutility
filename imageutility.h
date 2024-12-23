#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QMainWindow>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class ImageUtility;
}
QT_END_NAMESPACE

class ImageUtility : public QMainWindow
{
    Q_OBJECT

public:
    ImageUtility(QWidget *parent = nullptr);
    ~ImageUtility();

private:
    QPixmap* m_ImageFile = nullptr;
    Ui::ImageUtility *ui;
};
#endif // IMAGEUTILITY_H
