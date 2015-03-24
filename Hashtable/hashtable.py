class Data:
	def __init__(self, key, value, rev):
		self.key = key
		self.value = value
		self.rev = rev

class List_node:
	def __init__(self):
		self.next = None
		self.prev = None

class List:
	def __init__(self, List_node):
		self.head = List_node

class Entry:
	def __init__(self, List, Data):
		self.list = List
		self.data = Data

class Hashtable:
	def __init__(self, size, depth):
		self.size = size
		self.depth = depth
		self.buckets = []

	def hashtable_insert(self, Data):

	def hashtable_remove(self, key):

	def hashtable_lookup(self, key):

	def hashtable_update(self, Data):

def get_int_hash(key):
	hash_bits = (key * 2654435761)
	hash_bits = hash_bits % 2^32

	return hash_bits

def get_string_hash(key):
	hash_bits = 0
	for x in range(len(key)):
		hash_bits = hash_bits * 101 + int(key[x])

	return hash_bits

def get_slot(hash_num, depth):
	ret_val = hash_num >> (4 * depth) & 0xF

	return ret_val
