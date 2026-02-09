NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address,leak -g3
SANITIZE_FLAGS = -fsanitize=address -g3

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
INCLUDE = -I$(INCLUDE_DIR)

SRCS =  $(SRC_DIR)/main.cpp 			$(SRC_DIR)/cgi/CgiHandler.cpp \
	$(SRC_DIR)/config/ConfigParser.cpp	$(SRC_DIR)/config/Tokenizer.cpp \
	$(SRC_DIR)/core/Client.cpp 		$(SRC_DIR)/core/EventLoop.cpp \
	$(SRC_DIR)/core/Server.cpp 		$(SRC_DIR)/http/HttpRequest.cpp \
	$(SRC_DIR)/http/HttpResponse.cpp	$(SRC_DIR)/http/RequestParser.cpp \
	$(SRC_DIR)/http/Router.cpp 		$(SRC_DIR)/utils/FileUtils.cpp \
	$(SRC_DIR)/utils/MimeTypes.cpp		$(SRC_DIR)/utils/StringUtils.cpp \

OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

COMPILED_FLAG = $(BUILD_DIR)/.compiled
TOTAL = $(shell echo $(SRCS) | wc -w | tr -d " ")
TOTAL_PLUS = $(shell echo $$(($(TOTAL) + 3)))

all: $(COMPILED_FLAG)

$(COMPILED_FLAG): $(NAME)
	@mkdir -p $(BUILD_DIR)
	@touch $@  # Create the flag file in build directory

$(NAME): $(OBJS)
	@$(MAKE) --no-print-directory reset_progress
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo $$(( $(TOTAL_PLUS) - 1 )) > .progress_count
	@LAST=$$(cat .last_compiled 2>/dev/null || echo "") && \
	$(MAKE) --no-print-directory update_progress TARGET="$$LAST"
	@rm -f .progress_count .last_compiled
	@echo "\n\033[1;32m✔️ Build completed successfully\033[0m"

reset_progress:
	@echo 0 > .progress_count

update_progress:
	@if [ ! -f .progress_count ]; then echo 0 > .progress_count; fi; \
	count=$$(cat .progress_count); \
	count=$$((count + 1)); \
	echo $$count > .progress_count; \
	total=$(TOTAL_PLUS); \
	percent=$$((100 * count / total)); \
	width=40; \
	filled=$$((percent * width / 100)); \
	bar=$$(printf "%$${filled}s" | tr " " "#"); \
	spaces=$$(printf "%$$(($$width - $$filled))s"); \
	printf "\r\033[1;32mBuilding %-30s [%s%s] %d%%\033[0m" $(TARGET) "$$bar" "$$spaces" "$$percent"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp  # Fixed from %.c to %.cpp
	@mkdir -p $(dir $@)
	@echo "$<" > .last_compiled
	@$(MAKE) --no-print-directory update_progress TARGET="$<"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) .progress_count .last_compiled
	@$(MAKE) --no-print-directory clean_message

clean_message:
	@echo "\033[0;34mCleaned!\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;34mFully cleaned! 🗑️\033[0m"

re: fclean all

commit: fclean
	@git add .
	@./commit.sh
	@INPUT_VAR=$$(cat input.txt) && git commit -m "$(shell date +"%Y-%m-%d %H:%M:%S"):  $$INPUT_VAR" && rm -f input.txt
	@git push origin HEAD:main

valgrind:
	@valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(NAME)

sanitize:
	@$(MAKE) --no-print-directory clean
	@$(CC) $(CFLAGS) $(SANITIZE_FLAGS) $(INCLUDE) -o $(NAME) $(SRCS)
	@echo "\033[1;32m✔️ Sanitized build completed successfully\033[0m"

.PHONY: all clean fclean re commit valgrind update_progress reset_progress clean_message