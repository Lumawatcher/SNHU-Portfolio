from pymongo import MongoClient
from bson.objectid import ObjectId

class AnimalShelter(object):
    """Class for handling CRUD for Animal collectiong in MongoDB"""
    
    #Constructor accepts two parameters, for the username and password accessing the AAC database
    def __init__(self, username, password):
        #this is where we initialize the MongoClient, so we can access the db and collections
        self.client = MongoClient('mongodb://%s:%s@localhost:54064/?authSource=AAC' % (username, password))
        self.database = self.client['AAC']
        
    
    #This method accepts a python dictionary as a parameter and returns a boolean indicating whether the insert
    # function executed sucessfully
    def create(self, data):
        if data is not None:
            self.database.animals.insert(data)
            return True
            
        else:
            raise Exception("Nothing to save, because data parameter is empty")
            print(Exception)
            return False

    #This method accepts a python dictionary as a parameter, it expects the dictionary to be formatted in the MONGODB
    # query format (JSON). It returns a curosr on the first object in the query result
    def read(self, data):
        if data is not None:
            cursor = self.database.animals.find(data, {"_id":False})
            return cursor
        else:
            raise Exception("Nothing to display, data parameter is empty")
            
    
    #This method accepts two python dictionaries as parameters, the first being the query to be updated and the second
    # is the fields that should be updated. Both dictionaries should be formatted in the MONGODB format. The method returns
    # an UpdatedObject object from MONGODB with various information about the updated documents
    def update(self, search, update):
        if (search is not None) and (update is not None):
            x = self.database.animals.update_many(search, update)
            return x
        else:
            raise Execption("Cannot update, one or more parameters are empty")
            
    #This method accepts a python dictionary as a parameter, which is used as a query on the database. The reuslting list of
    # documents is dropped from the db and the method returns a DELETEDOBJECT object from MONGODB with various information
    # about the deleted objects
    def delete(self, query):
        if query is not None:
            x = self.database.animals.delete_many(query)
            return x
        else:
            raise Expection("Cannot delete, query parameter is empty")