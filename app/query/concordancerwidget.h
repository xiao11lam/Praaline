#ifndef CONCORDANCERWIDGET_H
#define CONCORDANCERWIDGET_H

#include <QMainWindow>

namespace Ui {
class ConcordancerWidget;
}

struct ConcordancerWidgetData;

class ConcordancerWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConcordancerWidget(QWidget *parent = 0);
    ~ConcordancerWidget();

private slots:
    void activeCorpusChanged(const QString &corpusID);

    void definitionOpen();
    void definitionSave();
    void metadataFilterAdd();
    void metadataFilterRemove();
    void queryAddFilterGroup();
    void queryAddFilterSequence();
    void queryAddFilterAttribute();
    void queryRemove();
    void displayLevelsAttributesAdd();
    void displayLevelsAttributesRemove();
    void resultsExport();
    void resultsSaveChanges();
    void resultsCreateBookmarks();

    void searchOccurrences();

private:
    Ui::ConcordancerWidget *ui;
    ConcordancerWidgetData *d;

    void setupActions();
};

#endif // CONCORDANCERWIDGET_H
