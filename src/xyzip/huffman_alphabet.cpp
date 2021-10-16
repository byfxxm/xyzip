#include "pch.h"
#include "huffman_alphabet.h"

huffman_alphabet::~huffman_alphabet()
{
	__destruct(__root);
}

optional<huffman_alphabet::code_ty> huffman_alphabet::operator[](letter_ty l)
{
	if (__alphabet.find(l) == __alphabet.end())
		return nullopt;

	return __alphabet.find(l)->second;
}

optional<huffman_alphabet::letter_ty> huffman_alphabet::operator[](code_ty c)
{
	if (__alphabet_rev.find(c) == __alphabet_rev.end())
		return nullopt;

	return __alphabet_rev.find(c)->second;
}

void huffman_alphabet::emplace_node(letter_ty l, weight_ty w)
{
	if (empty())
	{
		__root = new node(l, w);
		return;
	}

	auto new_root = new node(0, w + __root->weight);
	auto node_ = new node(l, w);

	if (w > __root->weight)
	{
		new_root->left = __root;
		new_root->right = node_;
	}
	else
	{
		new_root->right = __root;
		new_root->left = node_;
	}

	__root = new_root;
}

void huffman_alphabet::generate()
{
	if (empty())
		return;

	__generate(__root, "");
}

bool huffman_alphabet::empty()
{
	return __root == nullptr;
}

void huffman_alphabet::__generate(node* r, code_ty c)
{
	if (!r)
		return;

	if (!r->left && !r->right)
	{
		__alphabet[r->letter] = c;
		__alphabet_rev[c] = r->letter;
		return;
	}

	if (r->left)
		__generate(r->left, c + "0");

	if (r->right)
		__generate(r->right, c + "1");
}

void huffman_alphabet::__destruct(node* r)
{
	if (!r)
		return;

	if (r->left)
		__destruct(r->left);

	if (r->right)
		__destruct(r->right);

	delete r;
}