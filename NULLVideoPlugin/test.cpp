extern "C" void createPlugin(const char *name,void**a);

int main()
{
	void **a;
	createPlugin("NULLVideoPlugin",a);
	return 0;
}
