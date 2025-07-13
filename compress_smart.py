#!/usr/bin/env python3
import re

def smart_compress(code):
    # Phase 1: Essential variable compression only
    essential_vars = {
        'board': 'b',
        'side_to_move': 's', 
        'piece': 'p',
        'move': 'm',
        'from': 'f',
        'to': 't',
        'depth': 'd',
        'alpha': 'a',
        'beta': 'B',
        'score': 'S',
        'moves': 'M',
        'target': 'T',
        'legal_moves': 'L',
        'pseudo_moves': 'P'
    }
    
    for old_var, new_var in essential_vars.items():
        pattern = r'\b' + re.escape(old_var) + r'\b'
        code = re.sub(pattern, new_var, code)
    
    # Phase 2: Remove comments only
    code = re.sub(r'//.*', '', code)
    
    # Phase 3: Minimal whitespace reduction
    lines = code.split('\n')
    lines = [line.strip() for line in lines if line.strip()]
    
    # Only remove extra spaces, keep structure
    processed_lines = []
    for line in lines:
        # Keep necessary spaces for readability/compilation
        line = re.sub(r'\s+', ' ', line)
        line = re.sub(r'\s*{\s*', '{', line)
        line = re.sub(r'\s*}\s*', '}', line)
        processed_lines.append(line)
    
    return '\n'.join(processed_lines)

if __name__ == "__main__":
    with open('engine.cpp', 'r') as f:
        original_code = f.read()
    
    compressed = smart_compress(original_code)
    
    with open('engine_smart.cpp', 'w') as f:
        f.write(compressed)
    
    print(f"Original size: {len(original_code)} bytes")
    print(f"Smart compressed size: {len(compressed)} bytes")
    print(f"Compression ratio: {len(compressed)/len(original_code):.2%}")