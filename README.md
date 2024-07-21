# Simple Text Editor

## Overview

This is a simple text editor written in C. It provides basic functionalities for creating, editing, and saving text files. The project aims to demonstrate fundamental concepts in C programming, such as file handling, string manipulation, and user interface design.

## Features

- Open and edit existing text files
- Create and save new text files
- Basic text editing commands (insert, delete, find, replace)
- User-friendly command-line interface

## Requirements

- GCC compiler
- Make (optional, for easier compilation)
- A Unix-like environment (Linux, macOS) or a compatible terminal on Windows

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/simple-text-editor.git
    ```

2. Navigate to the project directory:
    ```sh
    cd simple-text-editor
    ```

3. Compile the source code:
    ```sh
    gcc -o text_editor main.c editor.c
    ```

    Alternatively, you can use the provided Makefile:
    ```sh
    make
    ```

## Usage

To run the text editor, use the following command:
```sh
./text_editor [filename]
```