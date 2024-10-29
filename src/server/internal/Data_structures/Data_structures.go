package datastructures

// Cryptography:

type AES struct {
	key []byte
	iv  []byte
}

type AES_Input struct {
	data      []byte
	timestamp int64
	filename  string
	id        string
}
