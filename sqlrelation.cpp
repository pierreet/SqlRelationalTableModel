/**************************************
*     SqlRelation Class for Qt        *
* Allow to make LEFT/OUTER/RIGHT join *
* Licence: Creatives Commons BY-NC-SA *
*    @author: Pierre-Etienne Crépy    *
*    @mail: pierre-etienne@crepy.eu   *
***************************************/

#include "sqlrelation.h"

SqlRelation::SqlRelation(const QString & tableName, const QString & indexColumn, const QString & displayColumn, const joinKind kind)
//    :QSqlRelation(tableName, indexColumn, displayColumn) no need to call parent constructor all needed methods and attributes are redefined
{
    setTableName(tableName);
    setiColumn(indexColumn);
    setdColumn(displayColumn);
    setJoinKind(kind);
}
SqlRelation::SqlRelation(const QSqlRelation & relation)
{
    SqlRelation(relation.tableName(), relation.indexColumn(), relation.displayColumn());
}
SqlRelation::SqlRelation(const SqlRelation & relation)
{
    SqlRelation(relation.tableName(), relation.indexColumn(), relation.displayColumn(), relation.jKind);
}
SqlRelation::SqlRelation()
{
    SqlRelation(QSqlRelation());
}

QString SqlRelation::tableName() const {
    return tName;
}

QString SqlRelation::indexColumn() const {
    return iColumn;
}

QString SqlRelation::displayColumn() const {
    return dColumn;
}

SqlRelation::joinKind SqlRelation::getJoinKind() const {
    return jKind;
}

void SqlRelation::setTableName(QString ptName){
    this->tName = ptName;
}

void SqlRelation::setiColumn(QString piColumn){
    this->iColumn= piColumn;
}

void SqlRelation::setdColumn(QString pdColumn){
    this->dColumn = pdColumn;
}

void SqlRelation::setJoinKind(joinKind pkind){
    this->jKind = pkind;
}


bool SqlRelation::isValid() const
{
    return !(tName.isEmpty() || iColumn.isEmpty() || dColumn.isEmpty());
}

SqlRelation SqlRelation::operator = (const SqlRelation &relation)
{
    this->setTableName(relation.tableName());
    this->setiColumn(relation.indexColumn());
    this->setdColumn(relation.displayColumn());
    this->setJoinKind(relation.getJoinKind());
    return *this;
}

