/*******************************************************************************
**
**  Copyright (C) 2009 J?rg Dentler
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this library.  If not, see <http://www.gnu.org/licenses/>.
**
*******************************************************************************/

#include <QtGui>
#include "qsegmentwidget.h"

#include <QDebug>

QSegmentWidget::QSegmentWidget(QWidget *parent):
  QWidget(parent),
  m_pixmap(0)
{
//  m_color = palette().color(foregroundRole());
    m_color = QColor(Qt::transparent);
}

void QSegmentWidget::InitPixmap()
{
  m_pixmap = new QPixmap(BM_OSIZE, BM_OSIZE);
//  m_pixmap->fill(palette().color(backgroundRole()));
  m_pixmap->fill(Qt::transparent);
  QPainter painter(m_pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setWindow( -(BM_OSIZE / 2), -(BM_OSIZE / 2),
    BM_OSIZE, BM_OSIZE);
//    qDebug() << QString("InitPixmap (%1, %2)").arg(-(BM_OSIZE / 2)).arg(BM_OSIZE);
  DrawBevel(&painter);
  DrawBackground(&painter);
//  DrawCenterText(&painter);
}

QSegmentWidget::~QSegmentWidget()
{
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::paintEvent(QPaintEvent *)
{
  if (!m_pixmap)
    InitPixmap();
  QPainter painter(this);
  int side = qMin(width(), height());
  painter.setViewport((width() - side) / 2, (height() - side) / 2,
    side, side);
  painter.setWindow(0, 0, BM_OSIZE, BM_OSIZE);
  painter.drawPixmap(0, 0, *m_pixmap);
//  QPainter paint(m_pixmap);
  DrawCenterText(&painter);
}

void QSegmentWidget::DrawBevel(QPainter *painter)
{
  painter->save();
//  QRadialGradient g(0, 0, BM_OSIZE / 2, 0, 0);
//  QPalette p = palette();
//  g.setColorAt(0.0, p.color(QPalette::Normal, QPalette::Midlight));
//  g.setColorAt(0.8, p.color(QPalette::Normal, QPalette::Dark));
//  g.setColorAt(0.9, p.color(QPalette::Normal, QPalette::Light));
//  g.setColorAt(1.0, p.color(QPalette::Normal, QPalette::Shadow));
//  painter->setBrush(g);

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  painter->setBrush(Qt::NoBrush);
  painter->setPen(Qt::NoPen);
  painter->drawEllipse(
    -((BM_SIZE + BM_BEVEL) / 2),
    -((BM_SIZE + BM_BEVEL) / 2),
    BM_SIZE + BM_BEVEL,
    BM_SIZE + BM_BEVEL);
//  qDebug() << QString("DrawBevel (%1, %2)").arg(-((BM_SIZE + BM_BEVEL) / 2)).arg(BM_SIZE + BM_BEVEL);
  painter->restore();
}

void QSegmentWidget::DrawCylinder(const Cylinder &cyl,
  const SegmentList &s, const bool erase)
{
  if (!m_pixmap)
    InitPixmap();
  qreal border = 0;
  QPainter painter(m_pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setWindow( -(BM_OSIZE / 2), -(BM_OSIZE / 2),
    BM_OSIZE, BM_OSIZE);
  QRectF rco(-cyl.outerRadius, -cyl.outerRadius, (cyl.outerRadius * 2.0)+border,
      (cyl.outerRadius * 2.0)+border);
  QRectF rci(-cyl.innerRadius, -cyl.innerRadius, (cyl.innerRadius * 2.0)+border,
      (cyl.innerRadius * 2.0)+border);
  QRegion ro(rco.toRect(), QRegion::Rectangle);
  QRegion ri(rci.toRect(), QRegion::Ellipse);
  QRegion region = ro.subtracted(ri);
  painter.setClipRegion(region);
  if (erase) {
      painter.setBrush(Qt::NoBrush);
      painter.setPen(Qt::NoPen);
  } else {
    QBrush b(cyl.color);
    painter.setBrush(b);
    painter.setPen(QPen(cyl.color.darker(200),border));
  }
  DrawSegments(&painter, s, cyl.outerRadius);
}

void QSegmentWidget::DrawSegments(QPainter *painter,
  const SegmentList &s, qreal radius)
{
  for (SegmentList::const_iterator i = s.begin(); i != s.end(); i++) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QBrush b(i->color);
    painter->setBrush(b);
    QBrush c(i->color.darker(200));
    painter->setPen(QPen(c, 0));
    int startAngle = qRound(i->first * 16.0);
    qreal dAngle = i->second - i->first;
    if (dAngle < 0.0)
      dAngle += 360.0;
    int spanAngle = qRound(dAngle * 16.0);
    QRectF r(-radius, -radius, radius * 2.0, radius * 2.0);
    if (spanAngle >= 360 * 16) {
//      qDebug() << QString("DrawSegments drawEllipse (%1, %2)").arg(-radius).arg(radius * 2.0);
      painter->drawEllipse(r);
    } else {
//      qDebug() << QString("DrawSegments drawPie (%1, %2, %3, %4)").arg(-radius).arg(radius * 2.0).arg(startAngle).arg(spanAngle);
      painter->drawPie(r, startAngle, spanAngle);
    }
  }
}

void QSegmentWidget::DrawBackground(QPainter *painter)
{
  painter->save();
//  QBrush face(m_color.darker(2 * 200));

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  QBrush face(Qt::NoBrush);
  painter->setBrush(face);
  painter->drawEllipse(-BM_RADIUS, -BM_RADIUS, BM_SIZE, BM_SIZE);
  painter->restore();
//  qDebug() << QString("DrawBackground (%1, %2)").arg(-BM_RADIUS).arg(BM_SIZE);
}

void QSegmentWidget::setColor(const QColor &c)
{
  m_color = c;
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::addCylinder(qreal size)
{
//    qDebug() << QString("addCylinder (%1)").arg(size);

  Q_ASSERT(size > 0.0);
  Cylinder cyl;
  cyl.size = size;
  cyl.color = m_color;
  m_cylinders.push_back(cyl);
  CalculateRadians();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::addCylinder(const QColor &c, qreal size)
{
//    qDebug() << QString("addCylinder (%1, %2)").arg(size).arg(c.name());
  Q_ASSERT(size > 0.0);
  Cylinder cyl;
  cyl.size = size;
  cyl.color = c;
  m_cylinders.push_back(cyl);
  CalculateRadians();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::clear()
{
//    qDebug() << QString("clean");

  m_cylinders.clear();
  delete m_pixmap;
  m_pixmap = 0;
}

void QSegmentWidget::CalculateRadians()
{
  qreal ssum = 0.0;
  for (QList<Cylinder>::iterator i = m_cylinders.begin();
      i != m_cylinders.end(); i++) {
    ssum += i->size;
  }
  qreal r = 0.0;
  for (QList<Cylinder>::iterator i = m_cylinders.begin();
      i != m_cylinders.end(); i++) {
    qreal s = (i->size / ssum) * BM_RADIUS;
    i->innerRadius = r;
    r += s;
    i->outerRadius = r;
  }
}

void QSegmentWidget::SetText(const int &value)
{
    m_textValue = value;
}

void QSegmentWidget::DrawCenterText(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setWindow( -(BM_OSIZE / 2), -(BM_OSIZE / 2), BM_OSIZE, BM_OSIZE);

    int fontHeight = 30;
    int shadowDepth = 3;
    int fontBorder = 1;
    QColor colorText = QColor(255,255,255);
    QColor colorBorder = QColor(200, 200, 200);
    QColor colorShadow = QColor(60,60,60);
    QFont fontText("Arial", fontHeight, 200);

    QString text = QString::number(m_textValue);
    QString symbol("%");

    QPainterPath pathText, pathTextShadow, pathSymbol, pathSymbolShadow;
    QPen pen;

    painter->setFont(fontText);

    int lenText = painter->fontMetrics().width(text);
    int lenSymb = painter->fontMetrics().width(symbol);

    pen.setWidth(fontBorder);
    pen.setColor(colorBorder);

    painter->setPen(colorShadow.darker(200));
    painter->setBrush(colorShadow);

    // shadow symbol
    painter->setTransform(QTransform(1.5, 0, 0, 1.5, 0, 0));
    pathSymbolShadow.addText(((lenText)/2)-(lenSymb/2)+(fontBorder*2)+3+shadowDepth, (fontHeight+shadowDepth), fontText, symbol);
    painter->drawPath(pathSymbolShadow);

    // shadow text
    painter->setTransform(QTransform(2.0, 0, 0, 3, 0, 0));
    pathTextShadow.addText(-((lenText+lenSymb)/2)+3+shadowDepth, (fontHeight/2)+shadowDepth, fontText, text);
    painter->drawPath(pathTextShadow);

    painter->setPen(pen);
    painter->setBrush(colorText);

    // symbol
    painter->setTransform(QTransform(1.5, 0, 0, 1.5, 0, 0));
    pathSymbol.addText(((lenText)/2)-(lenSymb/2)+(fontBorder*2)+3, (fontHeight), fontText, symbol);
    painter->drawPath(pathSymbol);

    // text
    painter->setTransform(QTransform(2.0, 0, 0, 3, 0, 0));
    pathText.addText(-((lenText+lenSymb)/2)+3, (fontHeight/2), fontText, text);
    painter->drawPath(pathText);

}

void QSegmentWidget::addSegment(int cyl, qreal startAngle, qreal stopAngle, QColor color)
{
//  Segment s(startAngle, stopAngle, color);
    QSegmentWidget::Segment s;
    s.first = startAngle;
    s.second = stopAngle;
    s.color = color;

  SegmentList sl;
  sl.push_back(s);
  addSegments(cyl, sl);
}

void QSegmentWidget::addSegments(int cyl, const SegmentList &s)
{
  Q_ASSERT(cyl >= 0 && cyl < m_cylinders.size());
  Cylinder &cylinder = m_cylinders[cyl];
  DrawCylinder(cylinder, s, false);
  update();
}

void QSegmentWidget::clearSegment(int cyl, qreal startAngle, qreal stopAngle)
{
  Segment s(startAngle, stopAngle, QColor(Qt::black));
  SegmentList sl;
  sl.push_back(s);
  clearSegments(cyl, sl);
}

void QSegmentWidget::clearSegments(int cyl, const SegmentList &s)
{
  Q_ASSERT(cyl >= 0 && cyl < m_cylinders.size());
  Cylinder &cylinder = m_cylinders[cyl];
  DrawCylinder(cylinder, s, true);
  update();
}

qreal QSegmentWidget::TrimAngle(const qreal &a)
{
  qreal r = a;
  while (r > 360.0)
    r -= 360.0;
  while (r < 0.0)
    r += 360.0;
  return r;
}


