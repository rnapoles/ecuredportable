/*
	Copyright (c) 2009, Aleksey Palazhchenko
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

		* Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following disclaimer
	in the documentation and/or other materials provided with the
	distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BreakpadHandler.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>

#if defined(Q_OS_MAC)
#include "client/mac/handler/exception_handler.h"
#elif defined(Q_OS_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(Q_OS_WIN32)
#include "client/windows/handler/exception_handler.h"
#endif
//#include <QDebug>


namespace BreakpadQt
{

class GlobalHandlerPrivate
{
public:
	GlobalHandlerPrivate();
	~GlobalHandlerPrivate();

public:
    //static char reporter_[1024];
    //static char reporterArguments_[8*1024];
	static google_breakpad::ExceptionHandler* handler_;
	static ReportCrashesToSystem reportCrashesToSystem_;
};

//char GlobalHandlerPrivate::reporter_[1024] = {0};
//char GlobalHandlerPrivate::reporterArguments_[8*1024] = {0};
google_breakpad::ExceptionHandler* GlobalHandlerPrivate::handler_ = 0;
ReportCrashesToSystem GlobalHandlerPrivate::reportCrashesToSystem_ = ReportUnhandled;


//bool launcher(const char* program, const char* const arguments[])
bool launcher(const QString & program, const QStringList & arguments)
{
	// TODO launcher
//	if(!GlobalHandlerPrivate::reporter_.isEmpty()) {
//		QProcess::startDetached(GlobalHandlerPrivate::reporter_);	// very likely we will die there
//	}

#if defined(Q_OS_WIN32)
    //path.append("ErrorReporter.exe");
#else
    //path.append("ErrorReporter");
#endif
    QString path =  QDir::toNativeSeparators(program);
    if(QFile::exists(path)){
        QProcess::startDetached(path,arguments);
    }
    qDebug()<<path;
    qDebug()<<arguments;
    //Q_UNUSED(program);
    //Q_UNUSED(arguments);
	return false;
}


#if defined(Q_OS_WIN32)
bool DumpCallback(const wchar_t* _dump_dir,
				const wchar_t* _minidump_id,
				void* context,
				EXCEPTION_POINTERS* exinfo,
				MDRawAssertionInfo* assertion,
				bool success)
#else
bool DumpCallback(const char* _dump_dir,
				const char* _minidump_id,
				void *context, bool success)
#endif
{
	Q_UNUSED(_dump_dir);
	Q_UNUSED(_minidump_id);
	Q_UNUSED(context);
#if defined(Q_OS_WIN32)
	Q_UNUSED(assertion);
	Q_UNUSED(exinfo);
#endif

#if defined(Q_OS_WIN32)
    //qDebug()<<"dump id"<<QString::fromWCharArray(_minidump_id,wcslen(_minidump_id));
    //qDebug()<<"_dump_dir"<<QString::fromWCharArray(_dump_dir,wcslen(_dump_dir));
    QString str_minidump_id = QString::fromWCharArray(_minidump_id,wcslen(_dump_dir));
    QString str__dump_dir = QString::fromWCharArray(_dump_dir,wcslen(_dump_dir));
#else
    QString str_minidump_id(_minidump_id);
    QString str__dump_dir(_dump_dir);
#endif


	/*
		NO STACK USE, NO HEAP USE THERE !!!
		Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
	*/


    BreakpadQt::GlobalHandler::instance()->emitSignal(str_minidump_id,str__dump_dir);
    //launcher(GlobalHandlerPrivate::reporter_, 0);
    QStringList  arguments;
    arguments << str__dump_dir;
    arguments << str_minidump_id;
    launcher(BreakpadQt::GlobalHandler::instance()->getReporter(),arguments);
	return (GlobalHandlerPrivate::reportCrashesToSystem_ == ReportUnhandled) ? success : false;
}


GlobalHandlerPrivate::GlobalHandlerPrivate()
{
    handler_ = new google_breakpad::ExceptionHandler(/*DumpPath*/ L".", /*FilterCallback*/ 0, DumpCallback, /*context*/ 0, google_breakpad::ExceptionHandler::HANDLER_ALL);
    /*ExceptionHandler(const wstring& dump_path,
                     FilterCallback filter,
                     MinidumpCallback callback,
                     void* callback_context,
                     int handler_types);*/
}

GlobalHandlerPrivate::~GlobalHandlerPrivate()
{
	delete handler_;
	handler_ = 0;
}


GlobalHandler* GlobalHandler::instance()
{
	static GlobalHandler globalHandler;
	return &globalHandler;
}

GlobalHandler::GlobalHandler()
{
	d = new GlobalHandlerPrivate();
}

GlobalHandler::~GlobalHandler()
{
	delete d;
	d = 0;
}

void GlobalHandler::setDumpPath(const QString& path)
{
    QString absPath = path;
	if(!QDir::isAbsolutePath(absPath)) {
		absPath = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + path);
        qDebug("BreakpadQt: setDumpPath: %s -> %s", qPrintable(path), qPrintable(absPath));
	}
    Q_ASSERT(QDir::isAbsolutePath(absPath));

    QDir().mkpath(absPath);
	Q_ASSERT(QDir().exists(absPath));

#	if defined(Q_OS_WIN32)
		d->handler_->set_dump_path(absPath.toStdWString());
#	else
		d->handler_->set_dump_path(absPath.toStdString());
#	endif

}

void GlobalHandler::setReporter(const QString& reporter)
{
    this->rep = reporter;

    if(!QDir::isAbsolutePath(this->rep)) {
#		if defined(Q_OS_MAC)
			// TODO(AlekSi) What to do if we are not inside bundle?
			rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/../Resources/") + rep);
#		elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
			// MAYBE(AlekSi) Better place for Linux? libexec? or what?
            this->rep = QDir::cleanPath(qApp->applicationDirPath() + QLatin1String("/") + rep);
#		else
			What is this?!
#		endif

		qDebug("BreakpadQt: setReporter: %s -> %s", qPrintable(reporter), qPrintable(rep));
	}
    Q_ASSERT(QDir::isAbsolutePath(this->rep));

	// add .exe for Windows if needed
#	if defined(Q_OS_WIN32)
        if(!QDir().exists(this->rep)) {
            this->rep += QLatin1String(".exe");
		}
#	endif
    Q_ASSERT(QDir().exists(this->rep));

//	qstrcpy(d->reporter_, QFile::encodeName(rep));
}

void GlobalHandler::setReportCrashesToSystem(ReportCrashesToSystem report)
{
	d->reportCrashesToSystem_ = report;
}

bool GlobalHandler::writeMinidump()
{
	bool res = d->handler_->WriteMinidump();
	if (res) {
		qDebug("BreakpadQt: writeMinidump() successed.");
	} else {
		qWarning("BreakpadQt: writeMinidump() failed.");
	}
    return res;
}

void GlobalHandler::emitSignal(const QString &id, const QString &path)
{
    emit Error(id,path);
}

QString GlobalHandler::getReporter() const
{
    return rep;
}




}	// namespace
