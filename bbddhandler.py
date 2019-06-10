import json
import sqlite3

# SQLite DB Name
DB_Name =  "bbdd.db"


class DatabaseManager():
    
	def __init__(self):       
		self.conn = sqlite3.connect(DB_Name)
		self.conn.execute('pragma foreign_keys = on')
		self.conn.commit()
		self.cur = self.conn.cursor()
        print("1")
		
	def add_del_update_db_record(self, sql_query, args=()):        
		self.cur.execute(sql_query, args)
		self.conn.commit()
		return
    
	def __del__(self):
		self.cur.close()
		self.conn.close()
    

def Data_Handler(topic,value,date,time):
	#Push into DB Table
	dbObj = DatabaseManager()
	dbObj.add_del_update_db_record("insert into Data (TopicName, Value, Date, time) values (?,?,?,?)",[topic, value, date, time])
	del dbObj
	print ("Inserted Data of topic "+ topic +" into Database.")
	print ("---------------------------------------------------")

def selectAll(sql_query, args=()):
    dbObj = DatabaseManager()
    dbObj.cur.execute(sql_query,args)
    return dbObj.cur.fetchall()