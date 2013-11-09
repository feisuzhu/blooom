#!/usr/bin/python
# -*- coding: utf-8 -*-

from setuptools import setup, Extension


blooomc = Extension(
    'blooom.blooomc',
    sources=[
        'blooom/bf.c',
        'blooom/siphash.c',
        'blooom/blooomc.c',
    ],
    libraries=['m'],
)


setup(
    name='blooom',
    version='0.0.2',
    url='http://www.github.com/feisuzhu/blooom',
    license='MIT',
    description='Bloomfilter library',
    author='Proton',
    author_email='proton@zhihu.com',
    packages=['blooom'],
    package_data={'blooom': ['*.h']},
    ext_modules=[blooomc],
    zip_safe=False,
)
