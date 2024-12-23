#include "imageutility.h"
#include "ui_imageutility.h"


#include <QFileDialog>
#include <QFile>
#include <QClipboard>
#include <QPropertyAnimation>
#include <QWidget>
#include <QStyle>

#include <QVariant>
#include <QtGlobal>

#include <QAndroidJniObject>

void openDocument() {
    QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V",
                             QAndroidJniObject::getStaticObjectField("android/content/Intent", "ACTION_OPEN_DOCUMENT", "Ljava/lang/String;").object<jstring>());

    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                            QAndroidJniObject::fromString("*/*").object<jstring>()); // Set the MIME type to accept all file types

    QAndroidJniObject activity = QtAndroid::androidActivity();
    activity.callMethod<void>("startActivityForResult", "(Landroid/content/Intent;I)V",
                              intent.object<jobject>(), 100); // 100 is a request code
}

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

#if defined(Q_OS_ANDROID)
        void MyActivity::onActivityResult(int requestCode, int resultCode, const QAndroidJniObject &data) {
            if (requestCode == 100 && resultCode == RESULT_OK) {
                QAndroidJniObject uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
                // Process the URI
            }
        }
#endif
        QFileDialog openDialog = QFileDialog(this,"Open Image File",homePath,fileFilter);
        //openDialog.setOption(QFileDialog::DontUseNativeDialog, true);
        openDialog.setFileMode(QFileDialog::ExistingFile);

        int result = openDialog.exec();
        if ( result != QDialog::Accepted ){
            ui->lblImage->setText("Some Kind of Magical Error Occurred: " + QString(QVariant(result).toString()));
            return;
        }

        QString fileName = openDialog.selectedFiles().first();

        if ( m_ImageFile ){
            delete m_ImageFile;
        }

        m_ImageFile = new QPixmap(fileName);

        // Set the image to be displayed
        QSize imageSize = ui->lblImage->size();
        ui->lblImage->setPixmap(m_ImageFile->scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

        showFadingMessage("Copied Image To Clipboard",ui->lblImage);
    });


    ui->btnClose->connect(ui->btnClose, &QPushButton::clicked, qApp, &QCoreApplication::quit);
}

ImageUtility::~ImageUtility()
{
    delete ui;
}
