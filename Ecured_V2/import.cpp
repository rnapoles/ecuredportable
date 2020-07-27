#include "import.h"
#include "ui_import.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDir>
#include <QTime>
#include <QMouseEvent>
#include "searchengine.h"

Import::Import(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Import)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->progressBar->setValue(0);
    ui->lineEditSource->installEventFilter(this);
    ui->lineEditBackup->installEventFilter(this);

    connect(ui->pushButtonCancelar, SIGNAL(clicked(bool)), this, SLOT(reject()));
}

Import::~Import()
{
    delete ui;
}

void Import::on_pushButtonSource_clicked()
{
    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)+ '/' ;
#ifdef linux
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*");
#else
    QString fileName = QFileDialog::getOpenFileName(this,"Abrir",dir,"*.*");
#endif

    if(!fileName.isEmpty()) ui->lineEditSource->setText(fileName);

}

void Import::on_pushButtonBackup_clicked()
{
    QString dir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

#ifdef linux
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",dir,"*");
#else
    QString fileName = QFileDialog::getSaveFileName(this,"Guardar",dir,"*.*");
#endif


    if(!fileName.isEmpty()) ui->lineEditBackup->setText(fileName);

}

void Import::on_pushButtonAceptar_clicked()
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Critical);

    QFile file(ui->lineEditSource->text());
    if (!file.open(QIODevice::ReadOnly)) {
        msg.setText("La fuente documental a importar no se pudo abrir");
        if(msg.exec()){
            done(QDialog::Rejected);
        }
    } else {
        QByteArray DbHeaderf = file.read(16);
        QByteArray DbHeader("53514c69746520666f726d6174203300");

        if(!(DbHeaderf.toHex() == DbHeader)){
            msg.setText("La fuente documental a importar es inválida o esta dañada");
            if(msg.exec()){
                done(QDialog::Rejected);
            }
        }
    }

    QDir currentPath(".");
    QString currentFilePath = currentPath.absolutePath() +"/wikidb";

    QString DstFileName = ui->lineEditBackup->text() ;


    if (currentFilePath.compare(ui->lineEditSource->text())==0){
        QMessageBox::critical(this,"Error","La fuente y el destino no pueden ser el mismo fichero");
        done(QDialog::Rejected);
    }


    bool copia;
    if(!ui->checkBox->isChecked()){
        copia = copyFile(currentFilePath,DstFileName,true,true,"Haciendo salva de la fuente documental");
    } else {
        copia = true;
    }

    if(copia){
        ui->progressBar->setValue(25);
        DstFileName = ui->lineEditSource->text();
        copia = copyFile(DstFileName,currentFilePath,true,false,"Copiando nueva fuente documental");

        if(copia){
            ui->progressBar->setValue(50);
            setEnabled(false);
            ui->progressBar->setValue(100);
            setEnabled(true);
            done(QDialog::Accepted);
        }

    }
    done(QDialog::Rejected);
}



void Import::on_pushButtonCancelar_clicked()
{

}

//http://www.qtforum.org/article/2921/copyfile-function-for-qt.html
bool Import::copyFile(const QString &source, QString &destination, bool overwrite, bool move,const QString &msgstr) {

    QFileInfo sourceInfo = QFileInfo(source);
    QFileInfo destinationInfo = QFileInfo(destination);
    QDir DirDestination = destinationInfo.dir();
    QFileInfo DirDestinationInfo = QFileInfo(DirDestination.absolutePath());

    QString sourceInfoStr = sourceInfo.absoluteFilePath();
    QString destinationInfoStr = destinationInfo.absoluteFilePath();


    if (!sourceInfo.exists()) {
        QMessageBox::critical(this, tr("Error"), "Error al copiar - El fichero "+sourceInfo.fileName()+" no existe.");

        return false;
    } else if (!sourceInfo.isFile()) {
        QMessageBox::critical(this, "Error", "Error al copiar - "+sourceInfo.fileName()+" no es un fichero.");
        return false;
    } else if (!sourceInfo.isWritable()){
        QMessageBox::critical(this,"Error","Error al copiar - usted no tiene permisos de escritura sobre:"+sourceInfo.absoluteFilePath());
        return false;
    } else if (!DirDestination.exists()) {
        QMessageBox::critical(this,"Error","Error al copiar - Destino no encontrado:"+DirDestination.absolutePath());
        return false;
    } else if (!DirDestinationInfo.isWritable()){
        QMessageBox::critical(this,"Error","Error al copiar - usted no tiene permisos de escritura sobre:"+DirDestinationInfo.absoluteFilePath());
        return false;
    } else if (sourceInfoStr.compare(destinationInfoStr)==0){
        QMessageBox::critical(this,"Error","La fuente y el destino no pueden ser el mismo fichero");
        return false;
    }

    // if destination file exists and overwrite disabled - return false
    if (!overwrite && QFile::exists(destination)) {
        QMessageBox::critical(this, "Error","Error al copiar - El fichero destino ya existe, sobrescritura deshabilitada.");
        return false; // its neither file nor directory
    }

    QFile sourceFile(source);
    QFile destinationFile(destination);

    if (sourceFile.open(QIODevice::ReadOnly) && destinationFile.open(QIODevice::WriteOnly)) {

        QProgressDialog progress(msgstr,"Abortar",0,(ulong) sourceFile.size(), this);
        progress.setWindowModality(Qt::WindowModal);

        uint dataLength = 8388608;
        char * data = new char[dataLength];
        ulong completed = 0;
        progress.setValue(completed);
        while (!sourceFile.atEnd()) {
            QApplication::processEvents();
            completed += dataLength;
            destinationFile.write(data, sourceFile.read(data, dataLength));
            destinationFile.flush();
            progress.setValue(completed);
            if (progress.wasCanceled()) {
                destinationFile.remove();
                progress.setValue((ulong) sourceFile.size());
                sourceFile.close();
                destinationFile.close();
                delete[] data;
                return false;
            }
        }
        delete[] data;
        progress.setValue((ulong) sourceFile.size());
        if (move) {
            if (!sourceFile.remove()) {
                destinationFile.remove();
                sourceFile.close();
                destinationFile.close();
                QMessageBox::critical(this,"Error","Error al copiar - El fichero " + sourceInfo.fileName() + " no se pudo eliminar");
                return false;
            }
        }
        sourceFile.close();
        destinationFile.close();
    }
    else {
        sourceFile.close();
        destinationFile.close();
        QMessageBox::critical(this, "Error", "Seleccione la fuente documental a importar.");
        return false;
    }
    return true;
}

bool Import::eventFilter(QObject *obj, QEvent *event)
{


    if (obj == ui->lineEditSource) {
        if (event->type() == QEvent::MouseButtonPress) {
            on_pushButtonSource_clicked();

        }
    }

    if (obj == ui->lineEditBackup && ui->lineEditBackup->isEnabled() ) {
        if (event->type() == QEvent::MouseButtonPress) {
            on_pushButtonBackup_clicked();

        }
    }

    return QObject::eventFilter(obj, event);
}


void Import::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked()){
        ui->label_2->setDisabled(true);
        ui->lineEditBackup->setDisabled(true);
        ui->pushButtonBackup->setDisabled(true);
    } else {
        ui->label_2->setDisabled(false);
        ui->lineEditBackup->setDisabled(false);
        ui->pushButtonBackup->setDisabled(false);
    }
}
