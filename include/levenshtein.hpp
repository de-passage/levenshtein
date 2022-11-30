#pragma once

int levenshtein(const char* source, const char* target) noexcept;
int levenshtein(const char* source, int source_len, const char* target, int target_len) noexcept;
