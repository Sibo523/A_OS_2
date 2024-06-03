# Main Makefile

# Define subdirectories looks better in my opnion
Q1 = Question_1
Q2 = Question_2
 Q3 = Question_3
 Q4 = Question_4
 Q5 = Question_3.5
 Q6 = Question_6

# List of subdirectories
SUBDIRS = $(Q1) $(Q2) $(Q3) $(Q4) $(Q5) $(Q6)
# Default target
all: $(SUBDIRS)

# Phony cause PHONY
.PHONY: all $(SUBDIRS) clean

# make for each subdirectory
$(SUBDIRS):
	$(MAKE) -C $@

# Use all the subdrectories cleans
clean:
	for dir in $(SUBDIRS); do \
	    $(MAKE) -C $$dir clean; \
	done
