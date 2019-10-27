import sys
import struct

def divide_chunks(l, n): 
  # looping till length l 
  for i in range(0, len(l), n):  
    yield l[i:i + n]

chunks = []
output = 'FILE1'

chunk_limit = 8192-6

for i in sys.argv[1:]:
  if ':' in i:
    addr = i.split(':')[0]
    fname = i.split(':')[1]
    chunks.append((int(addr, 16), fname))
  else:
    output = i

last_flag = 0xFFFF

file_idx = 0
block_idx = 0

for (addr, fname) in chunks:
  with open(fname, 'rb') as fin:
    file_idx = file_idx + 1
    block_idx = 0
    block_addr = addr
    data_blocks = list(divide_chunks(fin.read(), chunk_limit))
    for block in data_blocks:
      block_idx = block_idx + 1
      if file_idx == len(chunks) and block_idx == len(data_blocks):
        last_flag = 0x0000
      print(f'----------------{file_idx}:{block_idx}--')
      print(f'filename: {output}')
      print(f'flag: {hex(last_flag)}')
      print(f'len: {hex(len(block))}')
      print(f'addr: {hex(block_addr)}')
      
      byte_data = bytearray(len(block) + 6)
      byte_data[:6] = struct.pack('>HHH', last_flag, len(block), block_addr)
      byte_data[6:] = block

      with open(output, 'wb') as fout:
        fout.write(byte_data)

      # setup for next segment
      block_addr = chunk_limit + block_addr
      last_char = chr(ord(output[-1]) + 1)
      output = output[:-1] + last_char

