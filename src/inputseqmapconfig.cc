// This file is part of Projecteur - https://github.com/jahnf/projecteur - See LICENSE.md and README.md
#include "inputseqmapconfig.h"

#include "inputseqedit.h"
#include "logging.h"

#include <QHeaderView>
#include <QKeyEvent>

// -------------------------------------------------------------------------------------------------
namespace  {
  const InputSeqMapConfig invalidItem_;
}

// -------------------------------------------------------------------------------------------------
InputSeqMapConfigModel::InputSeqMapConfigModel(QObject* parent)
  : InputSeqMapConfigModel(nullptr, parent)
{}

// -------------------------------------------------------------------------------------------------
InputSeqMapConfigModel::InputSeqMapConfigModel(InputMapper* im, QObject* parent)
  : QAbstractTableModel(parent)
  , m_inputMapper(im)
{
  m_inputSeqMapConfigs.resize(3);
}

// -------------------------------------------------------------------------------------------------
int InputSeqMapConfigModel::rowCount(const QModelIndex& parent) const
{
  return ( parent == QModelIndex() ) ? m_inputSeqMapConfigs.size() : 0;
}

// -------------------------------------------------------------------------------------------------
int InputSeqMapConfigModel::columnCount(const QModelIndex& /*parent*/) const
{
  return 3; // inputseq, mapping type, edit..
}

// -------------------------------------------------------------------------------------------------
Qt::ItemFlags InputSeqMapConfigModel::flags(const QModelIndex &index) const
{
  if (index.column() == InputSeqCol)
    return (QAbstractTableModel::flags(index) | Qt::ItemIsEditable); // & ~Qt::ItemIsSelectable;

  return QAbstractTableModel::flags(index) & ~Qt::ItemIsEditable;
}

// -------------------------------------------------------------------------------------------------
QVariant InputSeqMapConfigModel::data(const QModelIndex& index, int role) const
{
  if (index.row() >= static_cast<int>(m_inputSeqMapConfigs.size()))
    return QVariant();

  if (index.column() == InputSeqCol) {
    if (role == Qt::DisplayRole) {
      return (QString() << m_inputSeqMapConfigs[index.row()].sequence);
    } else if (role == Roles::InputSeqRole) {
      return QVariant::fromValue(m_inputSeqMapConfigs[index.row()].sequence);
    }
  }
  else if (index.column() == ActionTypeCol) {
    if (role == Qt::DisplayRole)
      return QString::number(m_inputSeqMapConfigs[index.row()].action);
  }

  return QVariant();
}

// -------------------------------------------------------------------------------------------------
QVariant InputSeqMapConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch(section)
    {
    case InputSeqCol: return tr("Input Sequence");
    case ActionTypeCol: return tr("Action Type");
    case ActionCol: return tr("Action");
    }
  }
  return QVariant();
}

// -------------------------------------------------------------------------------------------------
const InputSeqMapConfig& InputSeqMapConfigModel::configData(const QModelIndex& index) const
{
  if (index.row() >= static_cast<int>(m_inputSeqMapConfigs.size()))
    return invalidItem_;

  return m_inputSeqMapConfigs[index.row()];
}

// -------------------------------------------------------------------------------------------------
void InputSeqMapConfigModel::setInputSequence(const QModelIndex& index, const KeyEventSequence& kes)
{
  if (index.row() < static_cast<int>(m_inputSeqMapConfigs.size()))
  {
    auto& c = m_inputSeqMapConfigs[index.row()];
    if (c.sequence != kes)
    {
      c.sequence = kes;
      emit dataChanged(index, index, {Qt::DisplayRole, Roles::InputSeqRole});
    }
  }
}

// -------------------------------------------------------------------------------------------------
InputMapper* InputSeqMapConfigModel::inputMapper() const
{
  return m_inputMapper;
}

// -------------------------------------------------------------------------------------------------
void InputSeqMapConfigModel::setInputMapper(InputMapper* im)
{
  m_inputMapper = im;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
InputSeqMapTableView::InputSeqMapTableView(QWidget* parent)
  : QTableView(parent)
{
  verticalHeader()->setHidden(true);
  const auto imSeqDelegate = new InputSeqDelegate(this);
  setItemDelegateForColumn(InputSeqMapConfigModel::InputSeqCol, imSeqDelegate);

  setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
}

// -------------------------------------------------------------------------------------------------
void InputSeqMapTableView::setModel(QAbstractItemModel* model)
{
  QTableView::setModel(model);

  if (const auto imModel = qobject_cast<InputSeqMapConfigModel*>(model)) {
    horizontalHeader()->setSectionResizeMode(InputSeqMapConfigModel::ActionTypeCol, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(InputSeqMapConfigModel::ActionCol, QHeaderView::ResizeToContents);
  }
}


//-------------------------------------------------------------------------------------------------
void InputSeqMapTableView::keyPressEvent(QKeyEvent* e)
{
  switch (e->key()) {
  case Qt::Key_Enter:
  case Qt::Key_Return:
    if (model()->flags(currentIndex()) & Qt::ItemIsEditable) {
      edit(currentIndex());
      return;
    }
    break;
  case Qt::Key_Delete:
    if (const auto imModel = qobject_cast<InputSeqMapConfigModel*>(model())) {
      imModel->setInputSequence(currentIndex(), KeyEventSequence{});
      return;
    }
    break;
  }

  QTableView::keyPressEvent(e);
}

