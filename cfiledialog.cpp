#include "cfiledialog.h"
#include "ui_cfiledialog.h"

#include <QFile>
#include <QFileInfo>

#include <QStandardPaths>
#include <QStringListModel>
#include <QStandardItemModel>


void changeWidgetFontColor(QWidget* pWidget, QColor color)
{
    if ( !pWidget ){
        return;
    }

    QPalette palette = pWidget->palette();
    palette.setColor(QPalette::WindowText, color);
    pWidget->setPalette(palette);
}

void populateComboBoxWithKnownPaths(QComboBox* pComboBox){
    if ( !pComboBox ){
        return;
    }

#if defined(Q_OS_ANDROID)
    QStringList knownPaths;
    knownPaths.push_back(QString("/storage/emulated/0"));
#else
    QStringList knownPaths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
#endif

    QStringListModel *model;
    model = new QStringListModel(pComboBox);

    model->setStringList(knownPaths);
    pComboBox->setModel(model);
}


QDir::SortFlags CFileDialog::getSortFlags(){
    QDir::SortFlags sortFlags = QDir::DirsFirst;

    if ( ui->btnSortName->isChecked() ){
        sortFlags |= QDir::Name;
    } else if ( ui->btnSortType->isChecked() ){
        sortFlags |= QDir::Type;
    } else if ( ui->btnSortDate->isChecked() ){
        sortFlags |= QDir::Time;
    }

    if ( ui->btnSortDsc->isChecked() ){
        sortFlags |= QDir::Reversed;
    }

    return sortFlags;
}

bool CFileDialog::updateFileListView(){
    //validate is an existing path
    if ( QDir(ui->cbPathList->currentText()).exists() == false ){
        return false;
    }

    this->m_directory = ui->cbPathList->currentText();
    // draw now just in case retrieving directory data takes too long
    ui->cbPathList->update();
    // get the directory data
    populateListViewWithDirectory(ui->lvDirectory, this->m_directory);

    return true;
}


void CFileDialog::populateListViewWithDirectory(QListView* pListView, QDir directory)
{
    if ( !pListView ){
        return;
    }

    QFileInfoList entries = directory.entryInfoList(QDir::NoFilter, getSortFlags());
    QStandardItemModel *model;

    model = new QStandardItemModel(0,1,pListView);

    for( QFileInfo entry: entries ){
        QStringList split = entry.fileName().split('.');
        QString ext = split[split.count()-1];
        QIcon icon;

        if ( entry.isFile() ){
            if ( ext == "doc" || ext == "xls" || ext == "txt" || ext == "log" || ext == "cfg" || ext == "ini" ){
                icon = m_fileIcons["document"];
            } else if ( ext == "bin" || ext == "exe" || ext == "so" || ext == "lib" ){
                icon = m_fileIcons["binary"];
            }else if ( ext == "mp3" || ext == "wav" || ext == "aac" || ext == "pfm" ){
                icon = m_fileIcons["music"];
            }else if ( ext == "mp4" || ext == "mov" || ext == "avi" || ext == "mkv" ){
                icon = m_fileIcons["video"];
            }else if ( ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" ){
                icon = m_fileIcons["picture"];
            } else {
                icon = m_fileIcons["unknown"];
            }
        } else if ( entry.isDir() ){
            icon = m_fileIcons["folder"];
        } else {
            icon = m_fileIcons["unknown"];
        }

        QStandardItem* pAddItem = new QStandardItem(icon,entry.fileName());
        model->appendRow(pAddItem);
    }
    pListView->setModel(model);
    pListView->setIconSize(QSize(32,32));

    //delete oldModel;
}



CFileDialog::CFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CFileDialog)
{
    //set the geometry before updating ui components
    if ( parent ){
        this->setGeometry(parent->geometry());
    }

    //setup ui components
    ui->setupUi(this);

    //get all resources to be used
    loadResources();

    // Populate Initial Data
    populateComboBoxWithKnownPaths(ui->cbPathList);
    populateListViewWithDirectory(ui->lvDirectory,QDir(ui->cbPathList->currentText()));

    // Setup Ui Events
    ui->lvDirectory->connect(ui->lvDirectory, &QListView::clicked, [this](const QModelIndex index){
        if ( !index.isValid() ){
            return;
        }

        m_filePath = "";
        m_fileName = "";

        QFileInfo fileInfo = QFileInfo(QDir(ui->cbPathList->currentText()), index.data().toString());

        if ( !fileInfo.exists() ){
            return;
        }

        // Change the directory
        if ( fileInfo.isDir() ){
            QDir newDir = QDir(fileInfo.canonicalFilePath());
            ui->cbPathList->setCurrentText(newDir.canonicalPath());
            populateListViewWithDirectory(ui->lvDirectory,newDir);
        } else { // Actual file data, record the data
            m_filePath = fileInfo.canonicalFilePath();
            m_fileName = fileInfo.fileName();
            m_directory = fileInfo.dir().canonicalPath();
        }
    });

    ui->cbPathList->connect(ui->cbPathList, &QComboBox::currentTextChanged, [this](){
        if ( ui->cbPathList->currentText() == "" ){
            return;
        }

        //validate is an existing path
        if ( updateFileListView() ){
            changeWidgetFontColor(ui->cbPathList, Qt::black);
        } else { // is not a valid path so gray out the text
            changeWidgetFontColor(ui->cbPathList, Qt::gray);
        }
    });

    // Setup Ui Buttons
    ui->btnSortName->connect(ui->btnSortName, &QPushButton::clicked, [this](){updateFileListView();});
    ui->btnSortType->connect(ui->btnSortType, &QPushButton::clicked, [this](){updateFileListView();});
    ui->btnSortDate->connect(ui->btnSortDate, &QPushButton::clicked, [this](){updateFileListView();});
    ui->btnSortAsc->connect(ui->btnSortAsc, &QPushButton::clicked, [this](){updateFileListView();});
    ui->btnSortDsc->connect(ui->btnSortDsc, &QPushButton::clicked, [this](){updateFileListView();});

    ui->btnCancel->connect(ui->btnCancel, &QPushButton::clicked, [this](){
        this->close();
    });

    ui->btnOkay->connect(ui->btnOkay, &QPushButton::clicked, [this](){
        if ( m_fileName == "" || m_filePath == "" ){
            this->reject();
            return;
        }

        this->accept();
    });

}


QString CFileDialog::filePath()
{
    return m_filePath;
}

QString CFileDialog::fileName()
{
    return m_fileName;
}

QString CFileDialog::fileDirectory()
{
    return m_directory;
}

void CFileDialog::loadResources(){
    m_fileIcons["binary"] = QIcon(":/image/filebinary");
    m_fileIcons["document"] = QIcon(":/image/filedocument");
    m_fileIcons["folder"] = QIcon(":/image/filefolder");
    m_fileIcons["music"] = QIcon(":/image/filemusic");
    m_fileIcons["picture"] = QIcon(":/image/filepicture");
    m_fileIcons["unknown"] = QIcon(":/image/fileunknown");
    m_fileIcons["video"] = QIcon(":/image/filevideo");

    //qDebug() << "Available Icon Sizes: " << m_fileIcons["video"].availableSizes();

#if defined(Q_OS_ANDROID)
    // Load the qlistview.css from the qrc resource
    QFile file(":/styles/qlistview.css");  // Path to the resource alias
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&file);
        QString css = ts.readAll();  // Read the content as a string

        QColor highlight = ui->lvDirectory->palette().highlight().color();
        QString highlightCss = QString("background: rgb(%0,%1,%2);").arg(highlight.red()).arg(highlight.green()).arg(highlight.blue());
        QString highlightCssDark = QString("background: rgb(%0,%1,%2);").arg(highlight.red()/1.5).arg(highlight.green()/1.5).arg(highlight.blue()/1.5);

        css = css.replace("background: rgb(0,0,0);", highlightCss);
        css = css.replace("background: rgb(1,1,1);", highlightCssDark);

        ui->lvDirectory->setStyleSheet(css); // Apply the CSS to trvFound
    }
#endif
}



CFileDialog::~CFileDialog()
{
    delete ui;
}
