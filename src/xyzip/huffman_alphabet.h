#pragma once
#include <optional>
#include <unordered_map>
#include <string>

using namespace std;

class huffman_alphabet
{
public:

	using letter_ty = unsigned char;
	using code_ty = string;
	using weight_ty = unsigned;

	struct node
	{
		node(letter_ty l, weight_ty w) : letter(l), weight(w) {}

		node* left = nullptr;
		node* right = nullptr;
		weight_ty weight = 0;
		letter_ty letter = 0;
	};

	~huffman_alphabet();
	optional<code_ty> operator[](letter_ty);
	optional<letter_ty> operator[](code_ty);
	void emplace_node(letter_ty, weight_ty);
	void generate();
	bool empty();

private:
	void __generate(node*, code_ty);
	void __destruct(node*);

	node* __root = nullptr;
	unordered_map<letter_ty, code_ty> __alphabet;
	unordered_map<code_ty, letter_ty> __alphabet_rev;
};