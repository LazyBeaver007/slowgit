#pragma once

#ifndef LAZY_GIT_CONFIG_HPP

#define LAZY_GIT_CONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

struct LazyGitConfig
{
	std::string folder_path;
	std::string remote_url;
	bool commit_after_save;
	int commit_interval_minutes; //0 to disable

	void save(const std::string& config_path);
	static LazyGitConfig load(const std::string& config_path);
};
#endif // !LAZY_GIT_CONFIG_HPP
