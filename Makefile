# Compilation is handled by CMake. This makefile is to run test scripts
# Refer to the README for more.
OUTDIR  = out
TARGET  = $(OUTDIR)/sim

clean:
	rm -f $(OUTDIR)/*

check: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) -m n-body -n 4 -t 5000 
