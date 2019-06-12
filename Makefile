.PHONY: clean All

All:
	@echo "----------Building project:[ FileTransfer - Debug ]----------"
	@"$(MAKE)" -f  "FileTransfer.mk"
clean:
	@echo "----------Cleaning project:[ FileTransfer - Debug ]----------"
	@"$(MAKE)" -f  "FileTransfer.mk" clean
