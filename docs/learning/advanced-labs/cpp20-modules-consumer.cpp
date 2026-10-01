import quote_values;

int main() {
	const ModuleQuote quote{193.50};
	return moduleClose(quote) == 193.50 ? 0 : 1;
}
