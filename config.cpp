#include "config.h"
#include<fstream>

using json = nlohmann::json;

void LazyGitConfig::save(const std::string& config_path)
{
	json j;
	j["folder_path"] = folder_path;
	j["remote_url"] = remote_url;
	j["commit_after_save"] = commit_after_save;
	j["commit_interval_minutes"] = commit_interval_minutes;

	std::ofstream file(config_path);
	file << j.dump(4);

}

LazyGitConfig LazyGitConfig::load(const std::string& config_path)
{
	LazyGitConfig config;
	try {
		std::ifstream file(config_path);
		json j;
		file >> j;
		config.folder_path = j["folder_path"];
		config.remote_url = j["remote_url"];
		config.commit_after_save = j["commit_after_save"];
		config.commit_interval_minutes = j["commit_interval_minutes"];

	}
	catch (...)
	{
		// return default config if file dosent exist
	}
	return config;
}