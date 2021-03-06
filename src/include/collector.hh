#pragma once

namespace kcov
{
	class IElf;

	class ICollector
	{
	public:
		class IListener
		{
		public:
			virtual void onAddress(unsigned long addr, unsigned long hits) = 0;
		};

		static ICollector &create(IElf *elf);

		virtual void registerListener(IListener &listener) = 0;


		virtual int run() = 0;

		virtual void stop() = 0;

		virtual ~ICollector() {};
	};
}
