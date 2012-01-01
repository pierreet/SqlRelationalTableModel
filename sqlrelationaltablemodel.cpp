/*********************************************
*Reimplemented Relational Table Model to     *
*allow left join, additionnal join and field,*
*group by and order by                       *
*and disable editable columns                *
*   Licence: Creatives Commons BY-NC-SA      *
*    @author: Pierre-Etienne Crépy           *
*    @mail: pierre-etienne@crepy.eu          *
***********************************************/

#include <QSqlRelationalTableModel>
#include <fonctions.h>
#include <SqlRelation.h>
#include "sqlrelationaltablemodel.h"

//public

  SqlRelationalTableModel::SqlRelationalTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db) {
      this->clear();
    }


  // Allow or not to edit the column
  void SqlRelationalTableModel::setEditableFlagForColumn(int column, bool editable)
  {
    if(!editable) {
      readOnlyColumns.insert(column);
    } else {
        readOnlyColumns.remove(column);
    }
  }

  Qt::ItemFlags SqlRelationalTableModel::flags ( const QModelIndex & index ) const
  {
    // retrieve parent class flags
    Qt::ItemFlags flags = QSqlRelationalTableModel::flags(index);

    // remove flage if column is in  readOnlyColumns
    if(readOnlyColumns.contains(index.column())) {
      flags &= ~Qt::ItemIsEditable;
    }

    return flags;
  }

  void SqlRelationalTableModel::clear()
  {
      QSqlRelationalTableModel::clear();
      groupByClause = QString::null;
      additionalField = QString::null;
  }

  void SqlRelationalTableModel::setGroupBy(QString grpby)
  {
      if(!grpby.isEmpty()&&!grpby.isNull())
          groupByClause = " GROUP BY " + grpby;
      else
          groupByClause = QString::null;
  }

  void SqlRelationalTableModel::setSort(int column, Qt::SortOrder order)
  {
      sortColumn = column;
      sortOrder = order;
  }

  void SqlRelationalTableModel::clearAdditionalField()
  {
      additionalField.clear();
  }

  void SqlRelationalTableModel::addAdditionalField(QString s)
  {
      additionalField.append(" ,"+s);
  }


  bool SqlRelationalTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
  {
      if ( role == Qt::EditRole && index.column() > 0 && index.column() < relations.count() && relations.value(index.column()).isValid()) {
          SRelation &relation = relations[index.column()];
          if (!relation.isDictionaryInitialized())
              relation.populateDictionary();
          if (!relation.dictionary.contains(value.toString()))
              return false;
      }
      return QSqlTableModel::setData(index, value, role);
  }

  void SqlRelationalTableModel::clearChanges()
  {
      for (int i = 0; i < relations.count(); ++i) {
          SRelation &rel = relations[i];
          rel.clear();
      }
  }

  void SqlRelationalTableModel::setRelation(int column, const SqlRelation &relation)
  {
      if (column < 0)
          return;
      if (relations.size() <= column)
          relations.resize(column + 1);
      relations[column].init(this, relation);
  }

  SqlRelation SqlRelationalTableModel::relation(int column) const
  {
      //need to return a new SqlRelation object else it doesnt work
      if(column<relations.size())
          return SqlRelation(relations[column].rel.tableName(),relations[column].rel.indexColumn(),relations[column].rel.displayColumn(),relations[column].rel.getJoinKind());
      else
          return SqlRelation();
  }

//protected

 QString SqlRelationalTableModel::selectStatement() const
  {
      QString query;

      if (tableName().isEmpty())
          return query;

      QString tList;
      QString jList;
      QString fList;
      QString where;

      QSqlRecord rec = record();
      QStringList tables;

      // Count how many times each field name occurs in the record
      QHash<QString, int> fieldNames;
      QStringList fieldList;
      for (int i = 0; i < rec.count(); ++i) {
          SqlRelation relation = this->relation(i);

          QString name;
          if (relation.isValid())
          {
              // Count the display column name, not the original foreign key
              name = relation.displayColumn();
              if (database().driver()->isIdentifierEscaped(name, QSqlDriver::FieldName))
                  name = database().driver()->stripDelimiters(name, QSqlDriver::FieldName);

              QSqlRecord rec = database().record(relation.tableName());
              for (int i = 0; i < rec.count(); ++i) {
                  if (name.compare(rec.fieldName(i), Qt::CaseInsensitive) == 0) {
                      name = rec.fieldName(i);
                      break;
                  }
              }
          }
          else
              name = rec.fieldName(i);
          fieldNames.insert(name, fieldNames.value(name, 0) + 1);
          fieldList.append(name);
      }

      for (int i = 0; i < rec.count(); ++i) {
          SqlRelation relation = this->relation(i);

          if (relation.isValid()) {
              QString relTableAlias = QString::fromLatin1("relTblAl_%1").arg(i);
              if (!fList.isEmpty())
                  fList.append(QLatin1String(", "));
              fList.append(relationField(relTableAlias,relation.displayColumn()));

              // If there are duplicate field names they must be aliased
              if (fieldNames.value(fieldList[i]) > 1) {
                  QString relTableName = relation.tableName().section(QChar::fromLatin1('.'), -1, -1);
                  if (database().driver()->isIdentifierEscaped(relTableName, QSqlDriver::TableName))
                      relTableName = database().driver()->stripDelimiters(relTableName, QSqlDriver::TableName);
                  QString displayColumn = relation.displayColumn();
                  if (database().driver()->isIdentifierEscaped(displayColumn, QSqlDriver::FieldName))
                      displayColumn = database().driver()->stripDelimiters(displayColumn, QSqlDriver::FieldName);
                  fList.append(QString::fromLatin1(" AS %1_%2_%3").arg(relTableName).arg(displayColumn).arg(fieldNames.value(fieldList[i])));
                  fieldNames.insert(fieldList[i], fieldNames.value(fieldList[i])-1);

                 }

              // this needs fixing!! the below if is borken.
              //tables.append(relation.tableName().append(QLatin1Char(' ')).append(relTableAlias));
              //if(!where.isEmpty())
                  //where.append(QLatin1String(" AND "));

              switch(relation.getJoinKind()){
              case SqlRelation::RIGHT:
                  jList.append(QLatin1String(" RIGHT JOIN "));
                  break;
              case SqlRelation::LEFT:
                  jList.append(QLatin1String(" LEFT JOIN "));
                  break;
              case SqlRelation::OUTER:
                  jList.append(QLatin1String(" FULL OUTER JOIN "));
                  break;
              case SqlRelation::INNER:
                  jList.append(QLatin1String(" INNER JOIN "));
                  break;
              default:
                  jList.append(QLatin1String(" JOIN "));
                  break;
                  }
              jList.append(relation.tableName().append(QLatin1Char(' ')).append(relTableAlias));
              jList.append(QLatin1String(" ON "));
              jList.append(relationField(tableName(), database().driver()->escapeIdentifier(rec.fieldName(i), QSqlDriver::FieldName)));
              jList.append(QLatin1String(" = "));
              jList.append(relationField(relTableAlias, relation.indexColumn()));
          } else {
              if (!fList.isEmpty())
                  fList.append(QLatin1String(", "));
              fList.append(relationField(tableName(), database().driver()->escapeIdentifier(rec.fieldName(i), QSqlDriver::FieldName)));
          }
      }

      for(int i = 0; i < rec.count(); ++i)
      {
          SqlRelation relation = this->relation(i);
          if(relation.isValid()){
              fList.append(QLatin1String(", "));
              fList.append(relationField(QString::fromLatin1("relTblAl_%1").arg(i),relation.indexColumn()));
              fList.append(QString::fromLatin1(" AS %1_%2").arg(relation.tableName().section(QChar::fromLatin1('.'), -1, -1)).arg(relation.indexColumn()));
          }
      }

      if(!additionalField.isEmpty())
          fList.append(additionalField+" ");

      if (!tables.isEmpty())
          tList.append(tables.join(QLatin1String(", ")));
      if (fList.isEmpty())
          return query;
      if(!tList.isEmpty())
          tList.prepend(QLatin1String(", "));
      tList.prepend(tableName());
      query.append(QLatin1String("SELECT "));
      query.append(fList).append(QLatin1String(" FROM ")).append(tList);
      query.append(jList);
      query = qAppendWhereClause(query, where, filter());
      query.append(groupByClause);

      QString orderBy = orderByClause();
      if (!orderBy.isEmpty())
          query.append(QLatin1Char(' ')).append(orderBy);

      return query;
  }

 QSqlTableModel * SqlRelationalTableModel::relationModel(int column) const
 {
     if ( column < 0 || column >= relations.count())
         return 0;

     SRelation &relation = relations[column];
     if (!relation.isValid())
         return 0;

     if (!relation.model)
         relation.populateModel();
     return relation.model;
 }

 QString SqlRelationalTableModel::orderByClause() const
 {
     const SqlRelation rel = relations.value(sortColumn).rel;
     if (!rel.isValid())
         return QSqlTableModel::orderByClause();

     QString s = QLatin1String("ORDER BY ");
     s.append(relationField(QLatin1String("relTblAl_") + QString::number(sortColumn),
                     rel.displayColumn()));
     s += sortOrder == Qt::AscendingOrder ? QLatin1String(" ASC") : QLatin1String(" DESC");
     return s;
 }
