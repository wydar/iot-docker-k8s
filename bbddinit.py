import sqlite3

# SQLite DB Name
DB_Name =  "bbdd.db"

# SQLite DB Table Schema
TableSchema="""
drop table if exists Data ;
create table Data (
  id integer primary key autoincrement,
  TopicName text,
  Value real,
  Date text,
  Time text
);
"""

#Connect or Create DB File
conn = sqlite3.connect(DB_Name)
curs = conn.cursor()

#Create Tables
sqlite3.complete_statement(TableSchema)
curs.executescript(TableSchema)

#Close DB
curs.close()
conn.close()