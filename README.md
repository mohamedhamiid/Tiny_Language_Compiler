# TINY Language Compiler

### Overview
This project is a **TINY Language Parser** that allows users to parse TINY language code, analyze its structure, and generate a syntax tree visualization.

**Download TINY Compiler**: [Download EXE](https://drive.google.com/file/d/1Z2qrmO4NIhf0cRx7f1LCN1hwkN8muVTg/view?usp=sharing)


## Main Features
- **Code Input**:Upload TINY language code or list of tokens file as `.txt`.
- **Scan**: Generate list of tokens by scanning the input code.
- **Parse**: Generate the syntax tree by parsing the input code.
- **Parse by Tokens**: Analyze TINY code structure by processing tokens directly.
- **Syntax Tree Visualization**: Visualize the syntax tree.
- **Rank Nodes**: Handle nodes of the same rank for clean tree representation.
- **Error Detection**: Identify syntax errors in the input code.
- **Empty Content Handling**: When no file imported, Parser and Scanner buttons are deactivated.

---
## Screenshots and testcases
### Browsing:
![browse](https://github.com/user-attachments/assets/da5bc0d9-9053-48f5-9dd4-e2afef4fc2a0)
### Importing code with lines numbered
![Code Imported](https://github.com/user-attachments/assets/a6d51f51-49d2-44ca-ba15-0ca966a52096)
### NO ERRORS Scenarios:
- The code is completely correct.
#### Run Scanner
![RunScanner](https://github.com/user-attachments/assets/62d59875-6160-445b-891c-d600fe526366)
#### Run Parser and Display Parse Tree
![RunParser](https://github.com/user-attachments/assets/dc74419e-65fc-4e1d-a83e-f230884ac101)
### ERROR in Scanning Scenarios:
- Adding '^' at line 6 (which is not supported in TINY) instead of '+'
#### Run Scanner
![Error in scanner](https://github.com/user-attachments/assets/e598d808-612b-426a-959e-d0e492b65d4d)
#### Run Parser
![RunParser  in scanner errors](https://github.com/user-attachments/assets/b0037a97-6afd-4f7e-9d5d-fc84ff0c04ee)
### ERROR in Parsing Scenarios:
- Replace 'if' with 'IF' at line 3, so 'IF' is considered as identifier expected ':=' after it.
#### Run Scanner
![Run Scanner in parser error](https://github.com/user-attachments/assets/c9c3e1f3-bcdc-462b-8ccc-60d4c2068b32)
#### Run Parser
![Error in parser](https://github.com/user-attachments/assets/dc2515e6-4b1d-4324-b381-c0277c6414bc)
---
### Imported list of tokens:
- The run parser button is deactivated when the tokens buttons is selected
![Tokens as input](https://github.com/user-attachments/assets/9f6a17fd-bba8-436c-8527-4d590e75ade8)
#### Run Parser
![RunParser when Iist of Tokens is input](https://github.com/user-attachments/assets/c2953952-d380-4f55-8a21-ce7a50065bc4)













---

## Contributors 
### GUI and Integration:
- Mohamed Abdel Hamid
- Hossam Tarek Elsayed
### Parser Logic:
- Anas Hamed Ahmed Hamed Ali
- Rana Amr Abdelnaby
### Scanner Logic
- Ahmed Gamal Helmy
- Mariam Ahmed Abd elgalil
- Ahmed Yousry Mohamed
