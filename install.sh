echo "   ___                   _              _____   _   __                     _ "
echo "  |_  |                 | |            /  __ \ | | / /                    | |"
echo "    | |_   _ _ __  _   _| |_ ___ _ __  | /  \/ | |/ /  ___ _ __ _ __   ___| |"
echo "    | | | | | '_ \| | | | __/ _ \ '__| | |     |    \ / _ \ '__| '_ \ / _ \ |"
echo "/\__/ / |_| | |_) | |_| | ||  __/ |    | \__/\ | |\  \  __/ |  | | | |  __/ |"
echo "\____/ \__._| .__/ \__. |\__\___|_|     \____/ \_| \_/\___|_|  |_| |_|\___|_|"
echo "            | |     __/ |                                                    "
echo "            |_|    |___/                                                     "


repo_name="jupyter-c-kernel"
repository=git@github.com:brendan-rius/jupyter-c-kernel.git

echo ":: Cloning Jupyter C-kernel... "
git clone $repository $repo_name; echo "Done. "
echo ":: Installing python module C kernel."
sudo -H pip install $repo_name; echo "Done. "
echo ":: Installing kernel specification"
cd $repo_name
sudo jupyter-kernelspec install c_spec/ ; echo "Done."
echo "Completed! Installation successful. You can type jupyter-notebook and be happy"
