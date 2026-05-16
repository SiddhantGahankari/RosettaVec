# Translation Model

This project implements a simple natural language translation system using pre-trained word embeddings. It supports basic English-to-French and English-to-Spanish word and sentence translation by leveraging cosine similarity between word vectors.

# Project Structure

ics_project/
## Header files 
├── include/ <br>
 ├── common.h  <br>
 ├── eval.h  <br>
 ├── globals.h <br>
 ├── io.h  <br>
 ├── translate.h<br> 
 │── vector.h <br>
## Source files <br>
├── src/
  ├── *_eval.c <br>
  ├── *_globals.c <br>
  ├── *_io.c <br>
  ├── *_main.c  <br>
  ├── *_translate.c <br>
  │── *_vector.c <br>
## Object files <br>
├── obj/ <br>
├── makefile # Build instructions <br>
├── main # Compiled binary (after build) <br>
├── .gitignore <br>
|── LICENSE<br>

## Build Instructions

If Data is not donwloaded Downlaod it from [here](https://drive.google.com/drive/folders/10quuOX3qur2va1fgnwWUcDwbaIW7i719?usp=drive_link)

Unzip & Add it to the /data folder 

Build the project using:

```bash
make
```

If this doesnt work use :
```bash
gcc -Iinclude src/*.c -o main -lm
```

##  Running Instructions

```bash
./main
```

## Features included in the project:

- Load and process word embeddings from files
- High-speed Approximate Nearest Neighbor Search using an **Inverted File Index (IVF)**
- K-means++ Clustering to divide vocabulary into optimal search spaces (O(V/K) latency)
- Translate individual words using top-k cosine similarity
- Translate entire sentences word-by-word
- Built-in benchmarking for measuring search speedup and accuracy recall
- Evaluate similarity and semantic closeness of translations
- Modular code architecture with clear separation of concerns


## Sources 

This project uses FastText word embeddings developed by Facebook AI Research (FAIR). Unlike Word2Vec, FastText represents words as character n-grams, allowing it to handle out-of-vocabulary words using subword information. We used pre-trained aligned word vectors trained on Wikipedia and Common Crawl, which place words from different languages (e.g., English, French, Spanish) in the same vector space. This enabled accurate word-level translation using cosine similarity to find semantically similar words across languages.
