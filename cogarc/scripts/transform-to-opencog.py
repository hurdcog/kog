#!/usr/bin/env python3
"""
transform-to-opencog.py - Transform existing code into an OpenCog component

This script analyzes an existing codebase and transforms it into an OpenCog
component following the standard archetype structure.
"""

import argparse
import os
import shutil
import sys
from pathlib import Path
import re
import subprocess
from datetime import datetime


class OpenCogTransformer:
    """Transform existing code into OpenCog component"""
    
    def __init__(self, source_dir, component_name, description, 
                 output_dir=None, dependencies="cogutil", version="1.0.0",
                 author="OpenCog Community", email="opencog@googlegroups.com"):
        self.source_dir = Path(source_dir).resolve()
        self.component_name = component_name
        self.description = description
        self.output_dir = Path(output_dir) if output_dir else Path.cwd()
        self.dependencies = dependencies.split(',')
        self.version = version
        self.author = author
        self.email = email
        
        # Derived names
        self.component_upper = component_name.upper().replace('-', '_')
        self.component_camel = ''.join(word.capitalize() 
                                        for word in component_name.split('-'))
        
        self.target_dir = self.output_dir / component_name
        
    def validate_source(self):
        """Validate source directory exists and contains code"""
        if not self.source_dir.exists():
            print(f"Error: Source directory {self.source_dir} does not exist")
            return False
            
        # Check for common source file extensions
        source_exts = ['.c', '.cc', '.cpp', '.h', '.hpp', '.cxx']
        has_source = False
        for ext in source_exts:
            if list(self.source_dir.glob(f'**/*{ext}')):
                has_source = True
                break
        
        if not has_source:
            print(f"Warning: No C/C++ source files found in {self.source_dir}")
            
        return True
    
    def analyze_source_structure(self):
        """Analyze existing source structure"""
        print(f"Analyzing source structure in {self.source_dir}...")
        
        analysis = {
            'source_files': [],
            'header_files': [],
            'test_files': [],
            'example_files': [],
            'has_cmake': False,
            'has_tests': False,
            'has_examples': False,
        }
        
        # Find files
        for ext in ['.c', '.cc', '.cpp', '.cxx']:
            analysis['source_files'].extend(self.source_dir.glob(f'**/*{ext}'))
            
        for ext in ['.h', '.hpp', '.hxx']:
            analysis['header_files'].extend(self.source_dir.glob(f'**/*{ext}'))
            
        # Look for test indicators
        for pattern in ['*test*.cpp', '*test*.cc', '*Test*', '*UTest*']:
            test_files = list(self.source_dir.glob(f'**/{pattern}'))
            if test_files:
                analysis['test_files'].extend(test_files)
                analysis['has_tests'] = True
                
        # Look for examples
        example_dir = self.source_dir / 'examples'
        if example_dir.exists():
            analysis['has_examples'] = True
            analysis['example_files'].extend(example_dir.glob('**/*.[ch]*'))
            
        # Check for CMakeLists.txt
        if (self.source_dir / 'CMakeLists.txt').exists():
            analysis['has_cmake'] = True
            
        return analysis
    
    def create_opencog_structure(self, analysis):
        """Create OpenCog directory structure"""
        print(f"Creating OpenCog structure in {self.target_dir}...")
        
        # Create directory structure
        dirs = [
            '.circleci',
            'cmake',
            'debian',
            'doc/doxydoc',
            'examples',
            'lib',
            f'opencog/{self.component_name}',
            'scripts',
            f'tests/{self.component_name}',
        ]
        
        for dir_path in dirs:
            (self.target_dir / dir_path).mkdir(parents=True, exist_ok=True)
            
    def copy_source_files(self, analysis):
        """Copy and organize source files"""
        print("Copying source files...")
        
        # Copy source and header files
        target_src = self.target_dir / 'opencog' / self.component_name
        
        # Copy header files
        for header in analysis['header_files']:
            if 'test' not in header.name.lower():
                rel_path = header.relative_to(self.source_dir)
                dest = target_src / rel_path.name
                shutil.copy2(header, dest)
                print(f"  Copied: {header.name} -> {dest.relative_to(self.target_dir)}")
                
        # Copy source files
        for source in analysis['source_files']:
            if 'test' not in source.name.lower():
                rel_path = source.relative_to(self.source_dir)
                dest = target_src / rel_path.name
                shutil.copy2(source, dest)
                print(f"  Copied: {source.name} -> {dest.relative_to(self.target_dir)}")
                
    def copy_test_files(self, analysis):
        """Copy test files"""
        if not analysis['has_tests']:
            print("No tests found to copy")
            return
            
        print("Copying test files...")
        target_test = self.target_dir / 'tests' / self.component_name
        
        for test in analysis['test_files']:
            dest = target_test / test.name
            shutil.copy2(test, dest)
            print(f"  Copied: {test.name} -> {dest.relative_to(self.target_dir)}")
            
    def copy_example_files(self, analysis):
        """Copy example files"""
        if not analysis['has_examples']:
            print("No examples found to copy")
            return
            
        print("Copying example files...")
        target_examples = self.target_dir / 'examples'
        
        for example in analysis['example_files']:
            rel_path = example.relative_to(self.source_dir / 'examples')
            dest = target_examples / rel_path
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(example, dest)
            print(f"  Copied: {example.name} -> {dest.relative_to(self.target_dir)}")
            
    def generate_component_using_script(self):
        """Use the generate-component.sh script"""
        print("Generating OpenCog component structure...")
        
        script_path = Path(__file__).parent / 'generate-component.sh'
        
        cmd = [
            str(script_path),
            self.component_name,
            self.description,
            '--output-dir', str(self.output_dir),
            '--version', self.version,
            '--author', self.author,
            '--email', self.email,
            '--dependencies', ','.join(self.dependencies),
        ]
        
        try:
            result = subprocess.run(cmd, check=True, capture_output=True, text=True)
            print(result.stdout)
            return True
        except subprocess.CalledProcessError as e:
            print(f"Error running generation script: {e}")
            print(e.stderr)
            return False
            
    def update_cmake_with_sources(self, analysis):
        """Update CMakeLists.txt with actual source files"""
        print("Updating CMakeLists.txt with source files...")
        
        cmake_path = self.target_dir / 'opencog' / self.component_name / 'CMakeLists.txt'
        
        # Get source and header file names (relative)
        source_files = [f.name for f in analysis['source_files'] 
                       if 'test' not in f.name.lower()]
        header_files = [f.name for f in analysis['header_files'] 
                       if 'test' not in f.name.lower()]
        
        # Read current CMakeLists.txt
        with open(cmake_path, 'r') as f:
            content = f.read()
            
        # Replace placeholders
        source_list = '\n\t'.join(source_files)
        header_list = '\n\t'.join(header_files)
        
        content = content.replace('# Add your source files here\n\t# example.cc',
                                 source_list if source_files else '# Add source files here')
        content = content.replace('# Add your header files here\n\t# example.h',
                                 header_list if header_files else '# Add header files here')
        
        with open(cmake_path, 'w') as f:
            f.write(content)
            
    def create_integration_guide(self):
        """Create a guide for integrating the transformed component"""
        guide_path = self.target_dir / 'INTEGRATION_GUIDE.md'
        
        guide_content = f"""# Integration Guide for {self.component_camel}

This component was automatically transformed from existing code into an
OpenCog component using the CogArc template system.

## What Was Done

1. ✓ Created standard OpenCog directory structure
2. ✓ Copied source files to `opencog/{self.component_name}/`
3. ✓ Copied test files to `tests/{self.component_name}/`
4. ✓ Generated CMake build configuration
5. ✓ Created Debian packaging files
6. ✓ Added CircleCI configuration

## What You Need to Do

### 1. Review Source Files

Check that all source files were copied correctly:
```bash
ls -la opencog/{self.component_name}/
```

### 2. Update Dependencies

Edit `CMakeLists.txt` to add any additional dependencies your code needs.

Current dependencies: {', '.join(self.dependencies)}

### 3. Update Version Header

Edit `opencog/{self.component_name}/version.h` if needed.

### 4. Add Missing Files

If the transformation missed any files, copy them manually:
```bash
cp /path/to/missing/file opencog/{self.component_name}/
```

### 5. Update Build Configuration

Review and update:
- `opencog/{self.component_name}/CMakeLists.txt` - Library configuration
- `tests/{self.component_name}/CMakeLists.txt` - Test configuration
- `examples/CMakeLists.txt` - Example configuration

### 6. Test the Build

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### 7. Run Tests

```bash
cd build
make check
```

### 8. Update Documentation

Edit `README.md` to:
- Add usage examples specific to your component
- Document API
- Add architecture notes

### 9. Customize Packaging

Edit files in `debian/` directory if you need custom packaging.

### 10. Set Up CI/CD

Configure CircleCI:
- Update `.circleci/config.yml` if needed
- Set up CircleCI project at https://circleci.com/

## Next Steps

1. Initialize git repository:
   ```bash
   git init
   git add .
   git commit -m "Initial OpenCog component structure"
   ```

2. Create GitHub repository and push:
   ```bash
   git remote add origin https://github.com/opencog/{self.component_name}.git
   git push -u origin master
   ```

3. Configure CI/CD in CircleCI

4. Build and test thoroughly

5. Submit to OpenCog for integration

## Troubleshooting

### Build Errors

- Check that all dependencies are installed
- Review CMakeLists.txt for correct library linking
- Ensure all header files are in the right location

### Test Failures

- Review test files in `tests/{self.component_name}/`
- Update test CMakeLists.txt to link required libraries
- Check that test data files are present

### Packaging Issues

- Review `debian/control` for correct dependencies
- Check `debian/*.install` files for correct paths
- Test package build: `cd build && cpack -G DEB`

## Support

For questions about OpenCog integration:
- Mailing list: opencog@googlegroups.com
- GitHub: https://github.com/opencog
- Wiki: https://wiki.opencog.org/

For questions about this transformation:
- Review the CogArc documentation in the occ repository
"""
        
        with open(guide_path, 'w') as f:
            f.write(guide_content)
            
        print(f"Created integration guide: {guide_path}")
        
    def transform(self):
        """Execute the transformation"""
        print(f"="*70)
        print(f"OpenCog Component Transformation")
        print(f"="*70)
        print(f"Source: {self.source_dir}")
        print(f"Component: {self.component_name}")
        print(f"Target: {self.target_dir}")
        print(f"="*70)
        print()
        
        # Validate
        if not self.validate_source():
            return False
            
        # Analyze source
        analysis = self.analyze_source_structure()
        print(f"\nFound:")
        print(f"  - {len(analysis['source_files'])} source files")
        print(f"  - {len(analysis['header_files'])} header files")
        print(f"  - {len(analysis['test_files'])} test files")
        print(f"  - Examples: {'Yes' if analysis['has_examples'] else 'No'}")
        print()
        
        # Generate component structure
        if not self.generate_component_using_script():
            print("Failed to generate component structure")
            return False
            
        # Copy files
        self.copy_source_files(analysis)
        self.copy_test_files(analysis)
        self.copy_example_files(analysis)
        
        # Update CMake
        self.update_cmake_with_sources(analysis)
        
        # Create guide
        self.create_integration_guide()
        
        print()
        print("="*70)
        print("✓ Transformation complete!")
        print("="*70)
        print()
        print(f"Component created in: {self.target_dir}")
        print()
        print("Next steps:")
        print(f"  1. cd {self.target_dir}")
        print(f"  2. Review INTEGRATION_GUIDE.md")
        print("  3. Test build: mkdir build && cd build && cmake .. && make")
        print()
        
        return True


def main():
    parser = argparse.ArgumentParser(
        description='Transform existing code into an OpenCog component',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic transformation
  %(prog)s --source /path/to/code --component-name my-component \\
    --description "My component description"
  
  # With additional options
  %(prog)s --source /path/to/code --component-name my-component \\
    --description "My component" --dependencies cogutil,atomspace \\
    --version 2.0.0 --author "John Doe" --email john@example.com
"""
    )
    
    parser.add_argument('--source', required=True,
                       help='Source directory containing existing code')
    parser.add_argument('--component-name', required=True,
                       help='Name for the OpenCog component (lowercase with hyphens)')
    parser.add_argument('--description', required=True,
                       help='Brief description of the component')
    parser.add_argument('--output-dir', default='.',
                       help='Output directory (default: current directory)')
    parser.add_argument('--dependencies', default='cogutil',
                       help='Comma-separated list of dependencies (default: cogutil)')
    parser.add_argument('--version', default='1.0.0',
                       help='Initial version (default: 1.0.0)')
    parser.add_argument('--author', default='OpenCog Community',
                       help='Author name (default: OpenCog Community)')
    parser.add_argument('--email', default='opencog@googlegroups.com',
                       help='Contact email (default: opencog@googlegroups.com)')
    
    args = parser.parse_args()
    
    # Validate component name
    if not re.match(r'^[a-z][a-z0-9-]*$', args.component_name):
        print("Error: Component name must be lowercase and contain only letters, numbers, and hyphens")
        sys.exit(1)
        
    # Create transformer and execute
    transformer = OpenCogTransformer(
        source_dir=args.source,
        component_name=args.component_name,
        description=args.description,
        output_dir=args.output_dir,
        dependencies=args.dependencies,
        version=args.version,
        author=args.author,
        email=args.email,
    )
    
    success = transformer.transform()
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
