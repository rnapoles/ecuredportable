#include "wikidbimporter.h"
#include <QDir>
#include <QFileInfo>

WikiDbImporter::WikiDbImporter(QObject *parent) :
    QObject(parent)
{
}

void WikiDbImporter::SetSource(const QString &s)
{
    this->source=s;
}

void WikiDbImporter::SetDestination(const QString &s)
{
    this->destination=s;
}


void WikiDbImporter::copyFile(const QString &source, QString &destination, bool overwrite, bool move){

    emit Init();
    QFileInfo sourceInfo = QFileInfo(source);
    QFileInfo destinationInfo = QFileInfo(destination);
    QDir DirDestination = destinationInfo.dir();
    QFileInfo DirDestinationInfo = QFileInfo(DirDestination.absolutePath());

    QString sourceInfoStr = sourceInfo.absoluteFilePath();
    QString destinationInfoStr = destinationInfo.absoluteFilePath();


    if (!sourceInfo.exists()) {
        emit Error("Error al copiar - El fichero "+sourceInfo.fileName()+" no existe.");
        emit Finish();
    } else if (!sourceInfo.isFile()) {        
        emit Error("Error al copiar - "+sourceInfo.fileName()+" no es un fichero.");
        emit Finish();
    } else if (!sourceInfo.isWritable()){        
        emit Error("Error al copiar - usted no tiene permisos de escritura sobre:"+sourceInfo.absoluteFilePath());
        emit Finish();
    } else if (!DirDestination.exists()) {        
        emit Error("Error al copiar - Destino no encontrado:"+DirDestination.absolutePath());
        emit Finish();
    } else if (!DirDestinationInfo.isWritable()){
        emit Error("Error al copiar - usted no tiene permisos de escritura sobre:"+DirDestinationInfo.absoluteFilePath());
        emit Finish();
    } else if (sourceInfoStr.compare(destinationInfoStr)==0){        
        emit Error("La fuente y el destino no pueden ser el mismo fichero");
        emit Finish();
    }
    if (!overwrite && QFile::exists(destination)) {        
        emit Error("Error al copiar - El fichero destino ya existe, sobrescritura deshabilitada.");
        emit Finish();
    }

    QFile sourceFile(source);
    QFile destinationFile(destination);

    if (sourceFile.open(QIODevice::ReadOnly) && destinationFile.open(QIODevice::WriteOnly)) {
        // show progress
        emit FileSize(sourceFile.size());
        uint dataLength = 8388608;
        char * data = new char[dataLength];
        ulong completed = 0;
        ReportProgress(completed);
        while (!sourceFile.atEnd()) {
            completed += dataLength;
            destinationFile.write(data, sourceFile.read(data, dataLength));
            destinationFile.flush();
            ReportProgress(completed);
        }
        delete[] data;
        ReportProgress((ulong) sourceFile.size());
        if (move) {
            if (!sourceFile.remove()) {
                destinationFile.remove();
                sourceFile.close();
                destinationFile.close();
                emit Error("Error al copiar - El fichero " + sourceInfo.fileName() + " no se pudo eliminar");
                emit Finish();
            }
        }
        sourceFile.close();
        destinationFile.close();

    }
    else {
        sourceFile.close();
        destinationFile.close();
        emit Error("Seleccione la fuente documental a importar.");
        emit Finish();
    }
    emit Finish();
}

void WikiDbImporter::copy()
{
    this->copyFile(this->source,this->destination,true,false);
}
