# Huffmam_Compression
C++ implementation of huffman compression achieving nearly 40% compression rates on text files.
Frequencies of alphabets are stored in a hashmap. The hashmap is translated into a priority queue with priority being decided by frequency of characters. The priority queue is used to construct the huffman tree. Encoding scheme derived from huffman tree and the file is compressed accordin to the derived encoding. 

For uncompressing the encoded file, the file is read in binary format and the huffman tree is repeatedly traversed based on the binary values (go to left child in case of a 0 and right child in case of a 1) till a left node is reached. Whenever the traversal leads to a leaf node, the corresponding character is picked up and writted to the uncompressed file.
