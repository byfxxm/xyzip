#include "huffman_alphabet.h"
#include "pch.h"

huffman_alphabet::~huffman_alphabet() { __destruct(__root); }

void huffman_alphabet::add_element(weight_ty w, letter_ty l) {
  if (empty()) {
    __root = new node(w, l);
    return;
  }

  auto new_root = new node(w + __root->weight);
  auto node_ = new node(w, l);

  if (w > __root->weight) {
    new_root->left = __root;
    new_root->right = node_;
  } else {
    new_root->right = __root;
    new_root->left = node_;
  }

  __root = new_root;
}

void huffman_alphabet::generate() {
  if (empty())
    return;

  __generate(__root, 0);
}

huffman_alphabet::code_ty huffman_alphabet::operator[](letter_ty l) {
  return __alphabet[l];
}

huffman_alphabet::letter_ty huffman_alphabet::operator[](code_ty c) {
  return __alphabet_rev[c];
}

bool huffman_alphabet::empty() { return __root == nullptr; }

void huffman_alphabet::__generate(node *r, code_ty c) {
  if (!r)
    return;

  if (!r->left && !r->right) {
    __alphabet[r->letter] = c;
    __alphabet_rev[c] = r->letter;
    return;
  }

  if (r->left)
    __generate(r->left, c << 1);

  if (r->right)
    __generate(r->right, (c << 1) + 1);
}

void huffman_alphabet::__destruct(node *r) {
  if (!r)
    return;

  if (r->left)
    __destruct(r->left);

  if (r->right)
    __destruct(r->right);

  delete r;
}