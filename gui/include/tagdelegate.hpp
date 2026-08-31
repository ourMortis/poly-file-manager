#ifndef TAGDELEGATE_HPP
#define TAGDELEGATE_HPP

#include <QStyledItemDelegate>
#include <QColor>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

/// Data role used to mark a tag item as "matches the selected file(s)".
inline constexpr int kTagHighlightRole = Qt::UserRole + 2;

/**
 * @brief Delegate for the tag list.
 *
 * Matching tags (marked with kTagHighlightRole) are always drawn with a solid
 * highlight background and bold text, independent of the selection/hover state,
 * so every matching tag is marked consistently in the dark theme.
 */
class TagDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit TagDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

  private:
    QColor highlight_color_ = QColor(38, 79, 120);
};

#endif // TAGDELEGATE_HPP
