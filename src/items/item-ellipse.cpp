/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

#include "item-ellipse.h"

#include "../painter.h"
#include "../core.h"

// ================================================================================
// =================== QCPItemEllipse
// ================================================================================

/*! \class QCPItemEllipse
  \brief An ellipse

  \image html QCPItemEllipse.png "Ellipse example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a topLeft and \a bottomRight, which define the rect the ellipse will be drawn in.
*/

/*!
  Creates an ellipse item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemEllipse::QCPItemEllipse(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  topLeft(createPosition("topLeft")),
  bottomRight(createPosition("bottomRight")),
  topLeftRim(createAnchor("topLeftRim", aiTopLeftRim)),
  top(createAnchor("top", aiTop)),
  topRightRim(createAnchor("topRightRim", aiTopRightRim)),
  right(createAnchor("right", aiRight)),
  bottomRightRim(createAnchor("bottomRightRim", aiBottomRightRim)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeftRim(createAnchor("bottomLeftRim", aiBottomLeftRim)),
  left(createAnchor("left", aiLeft))
{
  topLeft->setCoords(0, 1);
  bottomRight->setCoords(1, 0);
  
  setPen(QPen(Qt::black));
  setSelectedPen(QPen(Qt::blue, 2));
  setBrush(Qt::NoBrush);
  setSelectedBrush(Qt::NoBrush);
}

QCPItemEllipse::~QCPItemEllipse()
{
}

/*!
  Sets the pen that will be used to draw the line of the ellipse
  
  \see setSelectedPen, setBrush
*/
void QCPItemEllipse::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the ellipse when selected
  
  \see setPen, setSelected
*/
void QCPItemEllipse::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to fill the ellipse. To disable filling, set \a brush to
  Qt::NoBrush.
  
  \see setSelectedBrush, setPen
*/
void QCPItemEllipse::setBrush(const QBrush &brush)
{
  mBrush = brush;
}

/*!
  Sets the brush that will be used to fill the ellipse when selected. To disable filling, set \a
  brush to Qt::NoBrush.
  
  \see setBrush
*/
void QCPItemEllipse::setSelectedBrush(const QBrush &brush)
{
  mSelectedBrush = brush;
}

/* inherits documentation from base class */
double QCPItemEllipse::selectTest(const QPointF &pos) const
{
  double result = -1;
  QPointF p1 = topLeft->pixelPoint();
  QPointF p2 = bottomRight->pixelPoint();
  QPointF center((p1+p2)/2.0);
  double a = qAbs(p1.x()-p2.x())/2.0;
  double b = qAbs(p1.y()-p2.y())/2.0;
  double x = pos.x()-center.x();
  double y = pos.y()-center.y();
  
  // distance to border:
  double c = 1.0/qSqrt(x*x/(a*a)+y*y/(b*b));
  result = qAbs(c-1)*qSqrt(x*x+y*y);
  // filled ellipse, allow click inside to count as hit:
  if (result > mParentPlot->selectionTolerance()*0.99 && mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
  {
    if (x*x/(a*a) + y*y/(b*b) <= 1)
      result = mParentPlot->selectionTolerance()*0.99;
  }
  return result;
}

/* inherits documentation from base class */
void QCPItemEllipse::draw(QCPPainter *painter)
{
  QPointF p1 = topLeft->pixelPoint();
  QPointF p2 = bottomRight->pixelPoint();
  if (p1.toPoint() == p2.toPoint())
    return;
  QRectF ellipseRect = QRectF(p1, p2).normalized();
  QRect clip = clipRect().adjusted(-mainPen().widthF(), -mainPen().widthF(), mainPen().widthF(), mainPen().widthF());
  if (ellipseRect.intersects(clip)) // only draw if bounding rect of ellipse is visible in cliprect
  {
    painter->setPen(mainPen());
    painter->setBrush(mainBrush());
    try
    {
      painter->drawEllipse(ellipseRect);
    } catch (...)
    {
      qDebug() << Q_FUNC_INFO << "Item too large for memory, setting invisible";
      setVisible(false);
    }
  }
}

/* inherits documentation from base class */
QPointF QCPItemEllipse::anchorPixelPoint(int anchorId) const
{
  QRectF rect = QRectF(topLeft->pixelPoint(), bottomRight->pixelPoint());
  switch (anchorId)
  {
    case aiTopLeftRim:     return rect.center()+(rect.topLeft()-rect.center())*1/qSqrt(2);
    case aiTop:            return (rect.topLeft()+rect.topRight())*0.5;
    case aiTopRightRim:    return rect.center()+(rect.topRight()-rect.center())*1/qSqrt(2);
    case aiRight:          return (rect.topRight()+rect.bottomRight())*0.5;
    case aiBottomRightRim: return rect.center()+(rect.bottomRight()-rect.center())*1/qSqrt(2);
    case aiBottom:         return (rect.bottomLeft()+rect.bottomRight())*0.5;
    case aiBottomLeftRim:  return rect.center()+(rect.bottomLeft()-rect.center())*1/qSqrt(2);
    case aiLeft:           return (rect.topLeft()+rect.bottomLeft())*0.5;;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemEllipse::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemEllipse::mainBrush() const
{
  return mSelected ? mSelectedBrush : mBrush;
}
