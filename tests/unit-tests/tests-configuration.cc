#include "test.hh"

#include <configuration.hh>
#include <utils.hh>

#include "../../src/configuration.cc"

static bool runParse(std::string args)
{
	char *p = strdup(args.c_str());
	char *cur;
	std::list<std::string> argList;

	argList.push_back(std::string(crpcut::get_start_dir()) + "/test-binary");

	cur = strtok(p, " ");
	while (cur) {
		argList.push_back(std::string(cur));
		cur = strtok(NULL, " ");
	}
	free((void *)p);

	const char **argv = (const char **)malloc(argList.size() * sizeof(const char*));

	int i = 0;
	for (std::list<std::string>::iterator it = argList.begin();
			it != argList.end();
			it++) {
		// Yes, these are memory leaks.
		argv[i] = strdup((*it).c_str());
		i++;
	}

	return IConfiguration::getInstance().parse(argList.size(), argv);
}

TEST(configuration)
{
	std::string filename = std::string(crpcut::get_start_dir()) + "/test-binary";

	Configuration *conf = &(Configuration &)Configuration::getInstance();
	ASSERT_TRUE(conf);


	bool res = runParse(fmt("/tmp/vobb %s tjena", filename.c_str()));
	ASSERT_TRUE(res);

	const char **a = conf->getArgv();
	ASSERT_TRUE(filename == a[0]);
	ASSERT_TRUE(std::string("tjena") == a[1]);

	ASSERT_TRUE(conf->getBinaryName() == "test-binary");
	ASSERT_TRUE(conf->getBinaryPath() == std::string(crpcut::get_start_dir()) + "/");

	res = runParse("-h");
	ASSERT_FALSE(res);

	res = runParse("-s vbb");
	ASSERT_FALSE(res);

	ASSERT_TRUE(conf->getSortType() == IConfiguration::FILENAME);
	ASSERT_TRUE(conf->m_lowLimit == 25U);
	ASSERT_TRUE(conf->m_highLimit == 75U);

	res = runParse(fmt("-l 30,60 /tmp/vobb --sort-type=p %s", filename.c_str()));
	ASSERT_TRUE(res);

	ASSERT_TRUE(conf->getSortType() == IConfiguration::PERCENTAGE);
	ASSERT_TRUE(conf->m_lowLimit == 30U);
	ASSERT_TRUE(conf->m_highLimit == 60U);

	res = runParse(fmt("-l 20 /tmp/vobb --sort-type=p %s", filename.c_str()));
	ASSERT_TRUE(!res);

	res = runParse(fmt("-l 40,50,90 /tmp/vobb --sort-type=p %s", filename.c_str()));
	ASSERT_TRUE(!res);

	res = runParse(fmt("-l 35,hej /tmp/vobb --sort-type=p %s", filename.c_str()));
	ASSERT_TRUE(!res);

	res = runParse(fmt("-l yo,89 /tmp/vobb --sort-type=p %s", filename.c_str()));
	ASSERT_TRUE(!res);

	res = runParse(fmt("/tmp/vobb --sort-type=u %s", filename.c_str()));
	ASSERT_TRUE(res);
	ASSERT_TRUE(conf->getSortType() == IConfiguration::UNCOVERED_LINES);

	res = runParse(fmt("/tmp/vobb --sort-type=l %s", filename.c_str()));
	ASSERT_TRUE(res);
	ASSERT_TRUE(conf->getSortType() == IConfiguration::FILE_LENGTH);

	res = runParse(fmt("--path-strip-level=ejNummer /tmp/vobb, %s", filename.c_str()));
	ASSERT_FALSE(res);

	res = runParse(fmt("--path-strip-level=3 /tmp/vobb, %s", filename.c_str()));
	ASSERT_TRUE(res);

	res = runParse(fmt("--include-path=/a,/b/c --exclude-pattern=d/e/f /tmp/vobb %s", filename.c_str()));
	ASSERT_TRUE(res);

	ASSERT_TRUE(conf->getOnlyIncludePath().size() == 2U);
	ASSERT_TRUE(conf->getOnlyIncludePath()[0] == "/a");
	ASSERT_TRUE(conf->getOnlyIncludePath()[1] == "/b/c");

	ASSERT_TRUE(conf->getExcludePattern().size() == 1U);
	ASSERT_TRUE(conf->getExcludePattern()[0] == "d/e/f");

	res = runParse(fmt("--include-path=~/a /tmp/vobb %s", filename.c_str()));
	ASSERT_TRUE(res);

	ASSERT_TRUE(conf->getOnlyIncludePath().size() == 1U);
	ASSERT_TRUE(conf->getOnlyIncludePath()[0] == fmt("%s/a", get_home()));

	ASSERT_TRUE(conf->getAttachPid() == 0U);
	res = runParse(fmt("-p ejNummer /tmp/vobb %s", filename.c_str()));
	ASSERT_FALSE(res);

	res = runParse(fmt("-p 10 /tmp/vobb %s", filename.c_str()));
	ASSERT_TRUE(res);

	res = runParse(fmt("--pid=100 /tmp/vobb %s", filename.c_str()));
	ASSERT_TRUE(res);
	ASSERT_TRUE(conf->getAttachPid() == 100U);

	ASSERT_TRUE(g_kcov_debug_mask == 0);
	res = runParse(fmt("--debug=7 /tmp/vobb %s", filename.c_str()));
	ASSERT_TRUE(res);
	ASSERT_TRUE(g_kcov_debug_mask == 7);
}
