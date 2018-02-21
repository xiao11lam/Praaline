#include <QDebug>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>

#include "pncore/annotation/Interval.h"
using namespace Praaline::Core;

#include "AudioSegmenter.h"

namespace Praaline {
namespace Media {

// Audio Segmenter
// Input:  a wave file to extract segments from
//         the path where the extracts should be saved
//         a list of intervals: their tMin, tMax defines the segments to extract
//         attributeID -> this attribute will be used to name the extracted file (otherwise, will use text)
// Output: extracted wave files

// static
QList<QStringList> AudioSegmenter::script(const QString &filenameIn, const QString &pathOut, QList<Interval *> segments,
                                          const QString &attribute, uint newSamplingRate, bool normalise, int channels)
{
    QList<QStringList> script;
    int i = 1;
    foreach (Interval *seg, segments) {
        QStringList arguments;
        arguments << filenameIn;
        QString filename = (attribute.isEmpty()) ? seg->text() : seg->attribute(attribute).toString();
        if (filename.isEmpty()) filename = QString::number(i);
        if (normalise) {
            arguments << QString("--norm");
        }
        arguments << QString("%1/%2.wav").arg(pathOut).arg(filename);
        if (segments.count() > 0) {
            arguments << "trim" << QString::number(seg->tMin().toDouble(), 'g', 6) <<
                                   QString::number(seg->duration().toDouble(), 'g', 6);
        }
        if (channels > 0) {
            arguments << "channels" << QString("%1").arg(channels);
        }
        if (newSamplingRate) {
            arguments << "rate" << QString::number(newSamplingRate);
        }
        script << arguments;
        i++;
    }
    return script;
}


bool AudioSegmenter::segment(const QString &filenameIn, const QString &pathOut, QList<Interval *> segments,
                             const QString &attribute, uint newSamplingRate, bool normalise, int channels)
{
    QProcess sox;
    // DIRECTORY:
    QString soxPath;
#ifdef Q_OS_WIN
    QString appPath = QCoreApplication::applicationDirPath();
    soxPath = appPath + "/tools/sox/";
    sox.setWorkingDirectory(soxPath);
#else
    soxPath = "/usr/bin/";
#endif
    QString path = pathOut;
    if (path.endsWith("/")) path.chop(1);
    QList<QStringList> scriptArguments = script(filenameIn, pathOut, segments, attribute, newSamplingRate, normalise, channels);
    foreach (QStringList arguments, scriptArguments) {
        sox.start(soxPath + "sox", arguments);
        if (!sox.waitForFinished(-1)) // sets current thread to sleep and waits for sox end
            return false;
    }
    return true;
}

bool AudioSegmenter::bookmarkCutting(const QString &filenameIn, const QString &filenameOut, QList<Interval *> intervalsToRemove)
{
    QProcess sox;
    // DIRECTORY:
    QString soxPath;
#ifdef Q_OS_WIN
    QString appPath = QCoreApplication::applicationDirPath();
    soxPath = appPath + "/tools/sox/";
    sox.setWorkingDirectory(soxPath);
#else
    soxPath = "/usr/local/bin/";
#endif
    int i = 1;
    QMap<RealTime, Interval *> mapRemoveSorted;
    foreach (Interval *intv, intervalsToRemove)
        mapRemoveSorted.insert(intv->tMin(), intv);
    QMapIterator<RealTime, Interval *> iter(mapRemoveSorted);
    iter.toBack();
    while (iter.hasPrevious()) {
        iter.previous();
        QStringList arguments;
        // input file
        if (i == 1)
            arguments << filenameIn;
        else
            arguments << QString("temp%1.wav").arg(i);
        // output file
        if (i == intervalsToRemove.count())
            arguments << filenameOut;
        else
            arguments << QString("temp%1.wav").arg(i + 1);
        // trim command
        arguments << "trim 0 "
                  << QString("=%1").arg(QString::number(iter.value()->tMin().toDouble(), 'g', 6))
                  << QString("=%1").arg(QString::number(iter.value()->tMax().toDouble(), 'g', 6));
        // run sox for this interval
        qDebug() << soxPath + "sox" << arguments;
//        sox.start(soxPath + "sox", arguments);
//        if (!sox.waitForFinished(-1)) // sets current thread to sleep and waits for sox end
//            return false;
        // next
        i++;
    }
    return true;
}

// static
bool AudioSegmenter::resample(const QString &filenameIn, const QString &filenameOut,
                              uint newSamplingRate, bool normalise, int channels)
{
    QProcess sox;
    // DIRECTORY:
    QString soxPath;
#ifdef Q_OS_WIN
    QString appPath = QCoreApplication::applicationDirPath();
    soxPath = appPath + "/tools/sox/";
    sox.setWorkingDirectory(soxPath);
#else
    soxPath = "/usr/local/bin/";
#endif
    QStringList arguments;
    arguments << filenameIn;
    if (normalise) {
        arguments << QString("--norm");
    }
    arguments << filenameOut;
    if (channels > 0) {
        arguments << "channels" << QString("%1").arg(channels);
    }
    if (newSamplingRate) {
        arguments << "rate" << QString::number(newSamplingRate);
    }
    sox.start(soxPath + "sox", arguments);
    if (!sox.waitForFinished(-1)) // sets current thread to sleep and waits for sox end
        return false;
    return true;
}

} // namespace Media
} // namespace Praaline