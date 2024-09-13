"""
Check that the packages defined by find_package matches the ones in cmake/install-config.cmake
These are used by downstream dependencies and therefore helps users install the library.
"""


# %%
def main() -> None:
    """
    Check that the packages defined by find_package matches the ones in cmake/install-config.cmake
    """
    dependency_str_lines: list[str] = []

    with open("CMakeLists.txt", "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("find_package("):
                dependency_str_lines.append(line)

    dependencies = [line.split("(")[1].split(" ")[0] for line in dependency_str_lines]

    install_dependency_str_lines = []
    with open("cmake/install-config.cmake", "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("find_dependency("):
                install_dependency_str_lines.append(line)

    install_dependencies = [
        line.split("(")[1].split(")")[0] for line in install_dependency_str_lines
    ]

    assert set(install_dependencies) == set(dependencies), (
        f"Dependencies in find_package and install-config.cmake do not match.\n"
        f"find_package: {dependencies}\n"
        f"install-config.cmake: {install_dependencies}"
    )

    print(
        "Dependencies in find_package and install-config.cmake match."
        f"find_package: {dependencies}\n"
        f"install-config.cmake: {install_dependencies}"
    )


if __name__ == "__main__":
    main()
