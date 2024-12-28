#include "imageutility.h"
#include "ui_imageutility.h"

#include "cfiledialog.h"

#include <QDebug>

#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QClipboard>
#include <QMimeData>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>
#include <QWidget>

#include <QVariant>
#include <QtGlobal>


class FadingMessage : public QLabel {
public:
    FadingMessage(const QString& text, QWidget* parent = nullptr)
        : QLabel(text, parent) {
        setWordWrap(true);

        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        //setAttribute(Qt::WA_TranslucentBackground);
        setAlignment(Qt::AlignCenter);
        setStyleSheet("background-color: rgba(0, 0, 0, 150); color: white; border-radius: 5px;");

        // Center the message over the parent widget
        if (parent) {
            setGeometry(QStyle::alignedRect(Qt::LayoutDirectionAuto, Qt::AlignCenter, size(), parent->rect()));
        }

        // Create a fade-out animation
        QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(2000); // Fade out duration (milliseconds)
        animation->setStartValue(1.0);
        animation->setEndValue(0.0);
        animation->start();

        // Connect the animation finished signal to delete the message
        connect(animation, &QPropertyAnimation::finished, this, &FadingMessage::deleteLater);
    }
};

void showFadingMessage(const QString& text, QWidget* parent) {
    FadingMessage* message = new FadingMessage(text, parent);
    message->show();
}

ImageUtility::ImageUtility(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageUtility)
{
    ui->setupUi(this);

    ui->btnOpen->connect(ui->btnOpen, &QPushButton::clicked, [this](){

        QString fileFilter = "Image Files (*.png *.jpg *.bmp)";
        QString homePath = "/home/user";

        CFileDialog fileDialog(this);

        fileDialog.setModal(true);
        fileDialog.exec();
        int result = fileDialog.result();

        qDebug() << "result from dialog was: " << result << " recorded info: " << fileDialog.filePath();

        if ( result == QDialog::Rejected ){
            return;
        }

        QString filePath = fileDialog.filePath();

        if ( m_ImageFile ){
            delete m_ImageFile;
        }

        m_ImagePath = filePath;
        m_ImageFile = new QPixmap(filePath);

        updateGraphicsScene();

    });

    ui->btnCopy->connect(ui->btnCopy, &QPushButton::clicked, [this](){
        if ( !m_ImageFile ){
            return;
        }

        //////////////////////////////////////////////////////////////
        /// BUG:BUG
        /// Causes SIGABORT on:
        /// NAME="Linux Mint"
        ///     VERSION="22 (Wilma)"
        ///     VERSION_ID="22"
        ///     VERSION_CODENAME=wilma
        ///     6.8.0-51-generic
        ///
        /// QMake version 3.1
        /// Using Qt version 6.5.3 in [...]/Qt/6.5.3/gcc_64/lib
        //QApplication::clipboard()->setPixmap(*m_ImageFile,QClipboard::Clipboard);

        // Calling "setImage" doesn't produce SIGABORT bug
        QApplication::clipboard()->setImage(m_ImageFile->toImage(),QClipboard::Clipboard);
        showFadingMessage("Copied Image To Clipboard",ui->gvDisplay);

#if defined(Q_OS_ANDROID) // cannot copy image data without being a content provider (no raw pixel data) on android
        QMimeData* data = new QMimeData();
        data->setUrls(QList<QUrl>() << QUrl::fromLocalFile(m_ImagePath));
        showFadingMessage(data->text(),ui->gvDisplay);

        QApplication::clipboard()->setMimeData(data);
        QApplication::clipboard()->setText(data->text());
#endif

    });


    ui->btnClose->connect(ui->btnClose, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}


void ImageUtility::resizeEvent(QResizeEvent* event){
    QTimer::singleShot(50, this, &ImageUtility::updateGraphicsScene);

    QMainWindow::resizeEvent(event);
}

void ImageUtility::updateGraphicsScene()
{
    QRectF sceneRect = ui->gvDisplay->geometry().toRectF();
    //sceneRect.adjust(2,2,-4,-4);

    m_GraphicsScene.setSceneRect(sceneRect);
    m_GraphicsScene.clear();
    m_GraphicsScene.addRect(sceneRect,QPen(Qt::black),QBrush(Qt::black));

    // Set the image to be displayed
    if ( m_ImageFile ){
        QSize imageSize = sceneRect.size().toSize();
        //QGraphicsPixmapItem pixmap = QGraphicsPixmapItem(m_ImageFile->scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        //pixmap.setPos(sceneRect.width()/2,sceneRect.height()/2);
        m_GraphicsScene.addPixmap(m_ImageFile->scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        auto pixmap = m_GraphicsScene.items()[0];

        auto pixmaprect = pixmap->boundingRect();
        pixmap->setPos((sceneRect.width()-pixmaprect.width())/2, (sceneRect.height()-pixmaprect.height())/2);
    }

    ui->gvDisplay->setScene(&m_GraphicsScene);
    ui->gvDisplay->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gvDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gvDisplay->setContentsMargins(0,0,0,0);

    ui->gvDisplay->update();
}

ImageUtility::~ImageUtility()
{
    delete ui;
}
