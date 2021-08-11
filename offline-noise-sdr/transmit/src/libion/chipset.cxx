#include <fstream>
#include <ion/ion.h>

#define ALOGE(...) fprintf(stderr, __VA_ARGS__);

#define CHIPSET_MSM -1
#define CHIPSET_MEDIATEK 1
#define CHIPSET_EXYNOS 4
#define CHIPSET_MAKO 25
#define CHIPSET_TEGRA 2
#define CHIPSET_UNIVERSAL 1
#define CHIPSET_KIRIN 1
#define CHIPSET_SPREADTRUM 2
#define CHIPSET_QCT 22

int ion_magic_number(int fd, int len) {
  // inspired/taken from drammer
  int chipset = -1;
  int err = -1;
  ion_user_handle_t ion_handle;

  // attempt to guess ID from processor type
  chipset = CHIPSET_MSM;
  std::ifstream cpuinfo("/proc/cpuinfo");
  for (std::string line; getline(cpuinfo, line);) {
    if (line.find("Qualcomm") != std::string::npos) {
      printf("Detected chipset: Qualcomm\n");
      chipset = CHIPSET_MSM;
      break;
    }
    if (line.find("Exynos") != std::string::npos) {
      printf("Detected chipset: Exynos\n");
      chipset = CHIPSET_EXYNOS;
      break;
    }
    if (line.find(": 0x53") != std::string::npos) {
      printf("Detected chipset: Exynos\n"); // S7, S7 Edge, but probably more :(
      chipset = CHIPSET_EXYNOS;
      break;
    }
    if (line.find(": sc") != std::string::npos) {
      // Hardware : sc8830
      printf("Detected chipset: Spreadtrum\n");
      chipset = CHIPSET_SPREADTRUM;
      break;
    }
    if (line.find("EXYNOS") != std::string::npos) {
      // Samsung EXYNOS5433
      printf("Detected chipset: Exynos\n");
      chipset = CHIPSET_EXYNOS;
      break;
    }
    if (line.find("UNIVERSAL") != std::string::npos) {
      printf("Detected chipset: UNIVERSAL\n");
      chipset = CHIPSET_UNIVERSAL;
      break;
    }
    if (line.find("MAKO") != std::string::npos) {
      printf("Detected chipset: Mako\n");
      chipset = CHIPSET_MAKO;
      break;
    }
    if (line.find("Flounder") != std::string::npos) {
      printf("Detected chipset: Tegra\n");
      chipset = CHIPSET_TEGRA;
      break;
    }
    if (line.find(": MT") != std::string::npos) {
      printf("Detected chipset: Mediatek\n");
      chipset = CHIPSET_MEDIATEK;
      break;
    }
    if (line.find(": hi") != std::string::npos) {
      printf("Detected chipset Kirin\n");
      chipset = CHIPSET_KIRIN;
      break;
    }
    if (line.find("Kirin") != std::string::npos) {
      printf("Detected chipset Kirin\n");
      chipset = CHIPSET_KIRIN;
      break;
    }
    if (line.find("MSM8627") != std::string::npos) {
      printf("Detected cihpset MSM8627\n");
      chipset = CHIPSET_QCT;
    }
  }

  // chipset found
  if (chipset != -1) {
    // try anyhow before returning
    printf("Trying chipset %d\n", chipset);
    err = ion_alloc(fd, len, 0, (0x1 << chipset), 0, &ion_handle);
    if (err == 0) {
      // chipset found
      ion_free(fd, ion_handle);
      printf("Detected chipset %d\n", chipset);
      return chipset;
    }
  }

  // chipset not found, "bruteforcing"
  chipset = -1;
  while (chipset++ < 1000) {
    printf("Trying chipset %d\n", chipset);
    err = ion_alloc(fd, len, 0, (0x1 << chipset), 0, &ion_handle);
    if (err == 0) {
      // chipset found
      ion_free(fd, ion_handle);
      printf("Detected chipset %d\n", chipset);
      return chipset;
    }
  }

  // chipset not found
  return -1;
}
