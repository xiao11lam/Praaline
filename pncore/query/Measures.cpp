#include <QString>
#include <QPair>
#include <QList>
#include "annotation/IntervalTier.h"
#include "Measures.h"

namespace Praaline {
namespace Core {

// Helper static functions
// static
QPair<int, int> Measures::window(IntervalTier *tier, int i, int windowLeft, int windowRight, bool pauseBlocksWindow)
{
    QPair<int, int> ret;
    ret.first = i; ret.second = i;
    // Checks
    if (!tier) return ret;
    if (i < 0 || i >= tier->countItems()) return ret;
    if (tier->interval(i)->isPauseSilent() && pauseBlocksWindow) return ret;
    // Calculation
    ret.first = i - windowLeft;
    if (ret.first < 0) ret.first = 0;
    while ((ret.first < i) &&
           ((tier->interval(i)->isPauseSilent() && pauseBlocksWindow) || (!pauseBlocksWindow))) ret.first++;
    ret.second = i + windowRight;
    if (ret.second >= tier->countItems()) ret.second = tier->countItems() - 1;
    while ((ret.second > i) &&
           ((tier->interval(ret.second)->isPauseSilent() && pauseBlocksWindow) || (!pauseBlocksWindow))) ret.second--;
    return ret;
}

// static
bool Measures::mean(double &mean, IntervalTier *tier, QString attributeName, int i, int windowLeft, int windowRight,
                    bool pauseBlocksWindow, QString checkAttribute)
{
    if (!tier) return false;
    QPair<int, int> wnd = window(tier, i, windowLeft, windowRight, pauseBlocksWindow);
    double sum = 0.0;
    int count = 0;
    for (int j = wnd.first; j <= wnd.second; j++) {
        Interval *intv = tier->interval(j);
        if (!checkAttribute.isEmpty()) {
            if (intv->attribute(checkAttribute).toInt() == 0) continue; // check if attribute is non-zero
            // e.g. used to check if a syllable is stylised
        }
        double x = intv->attribute(attributeName).toDouble();
        sum = sum + x;
        count++;
    }
    if (count == 0) return false;
    mean = sum / ((double)count);
    return true;
}

// static
double Measures::mean(IntervalTier *tier, QString attributeName, int i, int windowLeft, int windowRight,
                      bool pauseBlocksWindow, QString checkAttribute)
{
    double value = 0.0;
    mean(value, tier, attributeName, i, windowLeft, windowRight, pauseBlocksWindow, checkAttribute);
    return value;
}

// static
double Measures::relative(IntervalTier *tier, QString attributeName, int i, int windowLeft, int windowRight,
                          bool pauseBlocksWindow, QString checkAttribute, bool logarithmic)
{
    // When it is impossible to calculate a relative value, return 1 for ratios or 0=log(1) for logarithmic attributes
    if (!tier) return (logarithmic) ? 0.0 : 1.0;
    Interval *intv = tier->interval(i);
    if (!intv) return (logarithmic) ? 0.0 : 1.0;
    // Check if attribute has to be stylised and if not, try to interpolate
    double value = intv->attribute(attributeName).toDouble();
    if ((!checkAttribute.isEmpty()) && (intv->attribute(checkAttribute).toInt() == 0)) {
        if (!mean(value, tier, attributeName, i, 1, 1, pauseBlocksWindow, checkAttribute))
            return (logarithmic) ? 0.0 : 1.0; // no luck
    }
    // Get mean in window
    double windowMean = 0.0;
    if (!mean(windowMean, tier, attributeName, i, windowLeft, windowRight, pauseBlocksWindow, checkAttribute))
        return (logarithmic) ? 0.0 : 1.0; // no luck
    // Calculate relative value
    if (logarithmic) {
        return value - windowMean;
    }
    // else linear
    return value / windowMean;
}


} // namespace Core
} // namespace Praaline