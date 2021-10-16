#pragma once

class huffman_alphabet
{
public:

	using letter_ty = unsigned char;
	using code_ty = unsigned;
	using weight_ty = unsigned;

	struct node
	{
		node(weight_ty w, letter_ty l) : weight(w), letter(l) {}
		node(weight_ty w) : weight(w) {}

		node* left = nullptr;
		node* right = nullptr;
		weight_ty weight = 0;
		letter_ty letter = 0;
	};

	~huffman_alphabet();
	void add_element(weight_ty, letter_ty);
	void generate();
	code_ty operator[](letter_ty);
	letter_ty operator[](code_ty);
	bool empty();

private:
	void __generate(node*, code_ty);
	void __destruct(node*);

	node* __root = nullptr;
	unordered_map<letter_ty, code_ty> __alphabet;
	unordered_map<code_ty, letter_ty> __alphabet_rev;
};