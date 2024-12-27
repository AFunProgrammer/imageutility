#ifndef IMAGEUTILITY_H
#define IMAGEUTILITY_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QResizeEvent>
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

protected:
    void resizeEvent(QResizeEvent* event) override;
    void updateGraphicsScene();

private:
    QString  m_ImagePath = "";
    QPixmap* m_ImageFile = nullptr;
    QGraphicsScene m_GraphicsScene;
    Ui::ImageUtility *ui;

};
#endif // IMAGEUTILITY_H
