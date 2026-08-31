#include "tagdelegate.hpp"

#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionViewItem>

TagDelegate::TagDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void TagDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const bool highlighted = index.data(kTagHighlightRole).toInt() == 1;
    if (!highlighted)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyleOptionViewItem o = option;
    initStyleOption(&o, index);

    painter->save();
    painter->fillRect(o.rect, highlight_color_);

    if (o.features & QStyleOptionViewItem::HasCheckIndicator)
    {
        QStyleOptionButton check;
        check.rect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &o, nullptr);
        check.state = QStyle::State_Enabled;
        if (index.data(Qt::CheckStateRole).toInt() == Qt::Checked)
        {
            check.state |= QStyle::State_On;
        }
        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &check, painter);
    }

    if (!o.icon.isNull())
    {
        const QRect icon_rect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &o, nullptr);
        o.icon.paint(painter, icon_rect, Qt::AlignCenter, QIcon::Normal);
    }

    QFont font = o.font;
    font.setBold(true);
    painter->setFont(font);
    const QRect text_rect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &o, nullptr);
    painter->setPen(QColor("#FFFFFF"));
    painter->drawText(text_rect, Qt::AlignVCenter | Qt::AlignLeft, o.text);

    painter->restore();
}

QSize TagDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 24));
    return size;
}
