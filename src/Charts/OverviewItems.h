/*
 * Copyright (c) 2020 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _GC_OverviewItem_h
#define _GC_OverviewItem_h 1

// basics
#include "ChartSpace.h"
#include "MetricSelect.h"
#include "DataFilter.h"
#include <QGraphicsItem>

// qt charts for zone chart
#include <QtCharts>
#include <QBarSet>
#include <QBarSeries>
#include <QLineSeries>

// subwidgets for viz inside each overview item
class RPErating;
class BPointF;
class Sparkline;
class BubbleViz;
class Routeline;
class ProgressBar;

// sparklines number of points - look back 6 weeks
#define SPARKDAYS 42

// number of points in a route viz - trial and error gets 250 being reasonable
#define ROUTEPOINTS 250

// types we use start from 100 to avoid clashing with main chart types
enum OverviewItemType { RPE=100, METRIC, META, ZONE, INTERVAL, PMC, ROUTE, KPI };

//
// Configuration widget for ALL Overview Items
//
class OverviewItemConfig : public QWidget
{
    Q_OBJECT

    public:

        OverviewItemConfig(ChartSpaceItem *);
        ~OverviewItemConfig();

    public slots:

        // retrieve values when user edits them (if they're valid)
        void dataChanged();

        // set the config widgets to reflect current config
        void setWidgets();

        // program editor
        void setErrors(QStringList &errors);

    private:

        // the widget we are configuring
        ChartSpaceItem *item;

        // editor for program
        DataFilterEdit *editor;
        QLabel *errors;

        // block updates during initialisation
        bool block;

        QLineEdit *name, *string1; // all of them
        QDoubleSpinBox *double1, *double2; // KPI
        MetricSelect *metric1, *metric2, *metric3; // Metric/Interval/PMC
        MetricSelect *meta1; // Meta
        SeriesSelect *series1; // Zone Histogram

};

class KPIOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        KPIOverviewItem(ChartSpace *parent, QString name, double start, double stop, QString program, QString units);
        ~KPIOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new KPIOverviewItem(parent, "CP", 0, 261, "{ 200; }", "watts"); }

        // settings
        double start, stop;
        QString program, units;

        // computed and ready for painting
        QString value;

        // progress bar viz
        ProgressBar *progressbar;
};

class RPEOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        RPEOverviewItem(ChartSpace *parent, QString name);
        ~RPEOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new RPEOverviewItem(parent, "RPE"); }

        // RPE meta field
        Sparkline *sparkline;
        RPErating *rperating;

        // for setting sparkline & painting
        bool up, showrange;
        QString value, upper, lower, mean;
};

class MetricOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        MetricOverviewItem(ChartSpace *parent, QString name, QString symbol);
        ~MetricOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new MetricOverviewItem(parent, "PowerIndex", "power_index"); }

        QString symbol;
        RideMetric *metric;
        QString units;

        bool up, showrange;
        QString value, upper, lower, mean;

        Sparkline *sparkline;
};

class MetaOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        MetaOverviewItem(ChartSpace *parent, QString name, QString symbol);
        ~MetaOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new MetaOverviewItem(parent, tr("Workout Code"), "Workout Code"); }

        QString symbol;
        int fieldtype;

        // for numeric metadata items
        bool up, showrange;
        QString value, upper, lower, mean;

        Sparkline *sparkline;
};

class PMCOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        PMCOverviewItem(ChartSpace *parent, QString symbol);
        ~PMCOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new PMCOverviewItem(parent, "coggan_tss"); }

        QString symbol;

        double sts, lts, sb, rr, stress;
};

class ZoneOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        ZoneOverviewItem(ChartSpace *parent, QString name, RideFile::seriestype);
        ~ZoneOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        void dragChanged(bool x);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new ZoneOverviewItem(parent, tr("Power Zones"), RideFile::watts); }

        RideFile::seriestype series;

        QChart *chart;
        QBarSet *barset;
        QBarSeries *barseries;
        QStringList categories;
        QBarCategoryAxis *barcategoryaxis;
};

class RouteOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        RouteOverviewItem(ChartSpace *parent, QString name);
        ~RouteOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new RouteOverviewItem(parent, tr("Route")); }

        Routeline *routeline;
};

class IntervalOverviewItem : public ChartSpaceItem
{
    Q_OBJECT

    public:

        IntervalOverviewItem(ChartSpace *parent, QString name, QString xs, QString ys, QString zs);
        ~IntervalOverviewItem();

        void itemPaint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
        void itemGeometryChanged();
        void setData(RideItem *item);
        QWidget *config() { return new OverviewItemConfig(this); }

        // create and config
        static ChartSpaceItem *create(ChartSpace *parent) { return new IntervalOverviewItem(parent, tr("Intervals"), "elapsed_time", "average_power", "workout_time"); }

        QString xsymbol, ysymbol, zsymbol;
        BubbleViz *bubble;
};


//
// below are theviz widgets used by the overview items
//

// for now the basics are x and y and a radius z, color fill
class BPointF {
public:

    BPointF() : x(0), y(0), z(0), fill(GColor(Qt::gray)) {}

    double score(BPointF &other);

    double x,y,z;
    QColor fill;
    QString label;
};


// bubble chart, very very basic just a visualisation
class BubbleViz : public QObject, public QGraphicsItem
{
    // need to be a qobject for metaproperties
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    // want a meta property for property animation
    Q_PROPERTY(int transition READ getTransition WRITE setTransition)

    public:
        BubbleViz(IntervalOverviewItem *parent, QString name=""); // create and say how many days
        ~BubbleViz();

        // we monkey around with this *A LOT*
        void setGeometry(double x, double y, double width, double height);
        QRectF geometry() { return geom; }

        // transition animation 0-255
        int getTransition() const {return transition;}
        void setTransition(int x) { if (transition !=x) {transition=x; update();}}

        // null members for now just get hooked up
        void setPoints(QList<BPointF>points);

        void setRange(double minx, double maxx, double miny, double maxy) {
            oldminx = this->minx;
            oldminy = this->miny;
            oldmaxx = this->maxx;
            oldmaxy = this->maxy;
            this->minx=minx;
            this->maxx=maxx;
            this->miny=miny;
            this->maxy=maxy;
        }
        void setAxisNames(QString xlabel, QString ylabel) { this->xlabel=xlabel; this->ylabel=ylabel; update(); }

        // needed as pure virtual in QGraphicsItem
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget*);
        QRectF boundingRect() const { return QRectF(parent->geometry().x() + geom.x(),
                                                    parent->geometry().y() + geom.y(),
                                                    geom.width(), geom.height());
        }

        // watch hovering
        bool sceneEvent(QEvent *event);

    private:
        IntervalOverviewItem *parent;
        QRectF geom;
        QString name;

        // where is the cursor?
        bool hover;
        QPointF plotpos;

        // for animated transition
        QList <BPointF> oldpoints; // for animation
        int transition;
        double oldmean;
        double oldminx,oldmaxx,oldminy,oldmaxy;
        QPropertyAnimation *animator;

        // chart settings
        QList <BPointF> points;
        double minx,maxx,miny,maxy;
        QString xlabel, ylabel;
        double mean, max;
};

// RPE rating viz and widget to set value
class RPErating : public QGraphicsItem
{

    public:
        RPErating(RPEOverviewItem *parent, QString name=""); // create and say how many days

        // we monkey around with this *A LOT*
        void setGeometry(double x, double y, double width, double height);
        QRectF geometry() { return geom; }

        void setValue(QString);

        // needed as pure virtual in QGraphicsItem
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget*);
        QRectF boundingRect() const { return QRectF(parent->geometry().x() + geom.x(),
                                                    parent->geometry().y() + geom.y(),
                                                    geom.width(), geom.height());
        }

        // for interaction
        bool sceneEvent(QEvent *event);
        void cancelEdit();
        void applyEdit();

    private:
        RPEOverviewItem *parent;
        QString name;
        QString description;
        QRectF geom;
        QString value, oldvalue;
        QColor color;

        // interaction
        bool hover;

};

// tufte style sparkline to plot metric history
class Sparkline : public QGraphicsItem
{
    public:
        Sparkline(QGraphicsWidget *parent, int count,QString name=""); // create and say how many days

        // we monkey around with this *A LOT*
        void setGeometry(double x, double y, double width, double height);
        QRectF geometry() { return geom; }

        void setPoints(QList<QPointF>);
        void setRange(double min, double max); // upper lower

        // needed as pure virtual in QGraphicsItem
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget*);
        QRectF boundingRect() const { return QRectF(parent->geometry().x() + geom.x(),
                                                    parent->geometry().y() + geom.y(),
                                                    geom.width(), geom.height());
        }

    private:
        QGraphicsWidget *parent;
        QRectF geom;
        QString name;
        double min, max;
        QList<QPointF> points;
};

// visualisation of a GPS route as a shape
class Routeline : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(int transition READ getTransition WRITE setTransition)

    public:
        Routeline(QGraphicsWidget *parent, QString name=""); // create and say how many days
        ~Routeline();

        // transition animation 0-255
        int getTransition() const {return transition;}
        void setTransition(int x) { if (transition !=x) {transition=x; update();}}

        // we monkey around with this *A LOT*
        void setGeometry(double x, double y, double width, double height);
        QRectF geometry() { return geom; }

        void setData(RideItem *item);

        // needed as pure virtual in QGraphicsItem
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget*);
        QRectF boundingRect() const { return QRectF(parent->geometry().x() + geom.x(),
                                                    parent->geometry().y() + geom.y(),
                                                    geom.width(), geom.height());
        }

    private:
        QGraphicsWidget *parent;
        QRectF geom;
        QString name;
        QPainterPath path, oldpath;
        double width, height; // size of painterpath, so we scale to fit on paint

        // animating
        int transition;
        QPropertyAnimation *animator;
        double owidth, oheight; // size of painterpath, so we scale to fit on paint
};

// progress bar to show percentage progress from start to goal
class ProgressBar : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(double value READ getCurrentValue WRITE setCurrentValue)

    public:
        ProgressBar(QGraphicsWidget *parent, double start, double stop, double value);
        ~ProgressBar();

        void setValue(double start, double stop, double value);

        // we monkey around with this *A LOT*
        void setGeometry(double x, double y, double width, double height);
        QRectF geometry() { return geom; }

        // needed as pure virtual in QGraphicsItem
        QVariant itemChange(GraphicsItemChange change, const QVariant &value);
        void paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget*);
        QRectF boundingRect() const { return QRectF(parent->geometry().x() + geom.x(),
                                                    parent->geometry().y() + geom.y(),
                                                    geom.width(), geom.height());
        }

        // transition animation between old and new values
        double getCurrentValue() const {return value;}
        void setCurrentValue(double x) { if (value !=x) {value=x; update();}}

    private:
        QGraphicsWidget *parent;
        QRectF geom;

        double start, stop, value;

        QPropertyAnimation *animator;
};
#endif // _GC_OverviewItem_h
